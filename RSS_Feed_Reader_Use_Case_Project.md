# RSS Feed Reader — Use Case Analysis

## 1. Problem statement

People who follow many websites — news sites, blogs, podcasts — have no easy way to know when any of them publish something new. Checking each site by hand takes time and articles get missed. An RSS feed reader solves this by letting a person subscribe to the sites they care about once, then automatically checking those sites in the background and showing all new articles in one place.

A reader signs up, adds feed subscriptions by pasting in a website's feed URL, and from then on sees a single, always-current list of new articles pulled from every site they follow. They can mark articles as read, search past articles, and unsubscribe from feeds that are no longer useful. Behind the scenes, the system checks every subscribed feed on a regular schedule, without the reader having to ask. If a feed becomes unreachable or starts returning malformed data, the system flags it so an administrator can investigate rather than letting the reader silently miss updates from that source.

An administrator is responsible for keeping the overall system healthy: they can see which feeds are failing to update, manage reader accounts, and remove feeds that are permanently broken or abusive. The system also depends on two things outside its own walls: the websites themselves, which publish the actual XML/JSON feed files the system reads, and a notification channel (email/push) used to alert people when something goes wrong.

The goal of the system is simple: readers should never have to visit a website just to check "is there anything new," and administrators should always know the health of every subscribed feed without manually testing each one.

## 2. Actors

| Actor | Type | Description |
|---|---|---|
| Reader | Primary human actor | Subscribes to feeds, reads articles, marks them read/unread, searches, unsubscribes |
| Administrator | Human actor | Manages reader accounts, monitors feed health, removes broken/abusive feeds |
| Scheduler | Time-triggered system actor | Fires automatically on a fixed interval (e.g. every 15 minutes) to initiate polling of every subscribed feed — no human starts this use case |
| Feed source (website) | External system | The publisher's server; hosts the XML/JSON feed file the system fetches over HTTP |
| Notification service | External system | Delivers email or push alerts when a feed fails or when other system events occur |

## 3. Use case diagram

See the diagram rendered in the conversation above the document. It shows the system boundary ("RSS feed reader system") containing eight use cases:

1. Subscribe to feed
2. Unsubscribe from feed
3. Validate feed URL
4. Poll feed for updates
5. Send failure notice
6. View article list
7. Mark article read
8. Monitor feed health

**Include relationship:** `Subscribe to feed` «include» `Validate feed URL` — every subscription attempt must validate the URL is a reachable, parseable feed before it's saved; this step is never optional.

**Extend relationship:** `Poll feed for updates` «extend» `Send failure notice`, guard condition `[fetch fails OR parse error]` — a failure notice is only generated when polling actually fails; on a normal successful poll this use case never fires.

**Actor associations:**
- Reader → Subscribe to feed, Unsubscribe from feed, View article list, Mark article read
- Scheduler → Poll feed for updates
- Administrator → Monitor feed health
- Notification service → Send failure notice (receives the outbound notification request)
- Feed source is invoked internally during Validate feed URL and Poll feed for updates (not shown with a direct association line to keep the diagram readable — mention this explicitly in your write-up if your instructor wants every external system drawn with a line)

## 4. Full use case specifications

---

### Use case 1: Subscribe to feed

**Primary actor:** Reader

**Stakeholders and interests:**
- *Reader* — wants to add a source to their feed list with minimal friction and be told immediately if the URL doesn't work
- *Administrator* — wants only valid, well-formed feeds entering the system, to avoid polluting the polling queue with dead URLs
- *Feed source* — wants only real requests, not repeated hits against an invalid endpoint

**Preconditions:**
- Reader is logged in
- Reader has a candidate feed URL (copied from a website)

**Postconditions (success):**
- The feed is stored under the reader's account
- The feed is added to the Scheduler's polling list
- Reader sees the feed in their subscription list

**Trigger:** Reader pastes a URL into the "add feed" field and submits.

**Main flow:**
1. Reader enters a feed URL and submits the subscribe form.
2. System executes «include» **Validate feed URL**: it sends an HTTP GET to the URL and confirms the response is valid RSS/Atom/JSON Feed XML or JSON.
3. System checks the reader isn't already subscribed to this exact feed.
4. System stores the feed record, associated with the reader's account.
5. System adds the feed to the active polling list used by the Scheduler.
6. System confirms success and displays the new feed in the reader's subscription list.

**Alternate flows:**

*A1 — URL is unreachable or times out (branches at step 2):*
1a. Validate feed URL returns a network failure.
2a. System displays "Couldn't reach this feed — check the URL and try again."
3a. Subscription is not created; reader remains on the add-feed form. Use case ends without success.

*A2 — Reader is already subscribed (branches at step 3):*
3a. System detects a duplicate subscription for this reader.
3b. System displays "You're already subscribed to this feed" and highlights the existing entry.
3c. Use case ends without creating a duplicate record.

*A3 — URL is reachable but not a valid feed format (branches at step 2):*
2a. Validate feed URL successfully connects but the response body isn't parseable RSS/Atom/JSON Feed.
2b. System displays "This doesn't look like a valid feed — try the site's RSS link instead."
2c. Use case ends without success.

---

### Use case 2: Poll feed for updates

**Primary actor:** Scheduler (time-triggered, not a human)

**Stakeholders and interests:**
- *Reader* — wants new articles to appear promptly and reliably, without having to trigger the check themselves
- *Administrator* — wants failing feeds surfaced quickly rather than silently going stale
- *Feed source* — wants polling to be well-behaved (reasonable frequency, not hammering the server)

**Preconditions:**
- At least one feed exists in the active polling list
- The interval since the feed's last poll has elapsed

**Postconditions (success):**
- Any new articles (identified by `<guid>`/`<pubDate>` not seen before) are stored and marked unread for every reader subscribed to that feed
- The feed's "last polled" timestamp is updated

**Trigger:** The Scheduler's timer fires (e.g. every 15 minutes) and selects the next feed due for polling.

**Main flow:**
1. Scheduler selects a feed whose polling interval has elapsed.
2. System sends an HTTP GET request to the feed's URL.
3. System parses the returned XML/JSON, extracting each item's `guid`, `title`, `link`, `pubDate`, and content.
4. System compares each item's `guid` against previously seen guids for this feed.
5. For each new item, system creates an article record and marks it unread for every reader subscribed to that feed.
6. System updates the feed's last-polled timestamp and moves to the next due feed.

**Alternate flows:**

*A1 — Fetch or parse fails (branches at step 2 or 3, triggers the extend point):*
2a. The HTTP request times out, returns a non-200 status, or the response body fails to parse as valid feed XML/JSON.
2b. System increments the feed's consecutive-failure count.
2c. System executes «extend» **Send failure notice** *[guard: fetch fails OR parse error]* — a notice is queued only when this condition is true.
2d. System leaves the last-polled timestamp unchanged and continues to the next feed in the queue. Use case ends without new articles for this feed.

*A2 — No new items found (branches at step 4):*
4a. Every item's guid already exists in storage.
4b. System updates the last-polled timestamp only; no article records are created and no readers are notified.

*A3 — Feed has exceeded its failure threshold (branches at step 2a):*
2a. Consecutive-failure count reaches a configured limit (e.g. 5 failed polls in a row).
2b. System marks the feed as "suspended" rather than queuing another notice for every failure.
2c. Feed is removed from the active polling list until an administrator reviews it (see Monitor feed health).

---

### Use case 3: Monitor feed health

**Primary actor:** Administrator

**Stakeholders and interests:**
- *Administrator* — wants a single view of every feed's status, to catch problems before readers complain
- *Reader* — indirectly benefits: broken feeds get fixed or removed instead of silently going stale
- *Feed source* — benefits if repeatedly-failing feeds get suspended rather than polled forever

**Preconditions:**
- Administrator is logged in with administrative privileges
- At least one feed exists in the system

**Postconditions (success):**
- Administrator has an up-to-date view of feed status (healthy / failing / suspended)
- Any action taken (suspend, remove, retry) is applied and logged

**Trigger:** Administrator opens the "feed health" dashboard.

**Main flow:**
1. Administrator navigates to the feed health dashboard.
2. System retrieves every feed's current status: last successful poll time, consecutive-failure count, and subscriber count.
3. System displays the list, sorted with failing feeds at the top.
4. Administrator reviews a failing feed's details, including the most recent failure notice.
5. Administrator selects "retry now" to force an immediate poll outside the normal schedule.
6. System re-executes Poll feed for updates for that single feed and updates its displayed status.

**Alternate flows:**

*A1 — Administrator removes a permanently broken feed (branches at step 4):*
4a. Administrator determines the feed is defunct (e.g. the site has shut down).
4b. Administrator selects "remove feed."
4c. System deletes the feed record, removes it from every subscriber's list, and logs the removal with the administrator's identity and timestamp.

*A2 — Retry succeeds and feed is reinstated (branches at step 6):*
6a. The forced retry poll succeeds.
6b. System resets the feed's consecutive-failure count to zero and, if it had been suspended, returns it to the active polling list.
6c. Dashboard updates to show the feed as healthy.

*A3 — No failing feeds exist (branches at step 3):*
3a. System detects every feed is currently healthy.
3b. System displays "All feeds healthy" with summary counts instead of a failure list.
3c. Use case ends; no further administrator action is required.

---

## Notes for write-up / submission

- If your rubric wants every external system actor drawn with an explicit association line (rather than described in prose, as done above for Feed source), add two lines from `Feed source` to `Validate feed URL` and `Poll feed for updates` on the diagram.
- The include relationship is mandatory-and-always-run (Validate feed URL runs on *every* subscribe attempt); the extend relationship is conditional (Send failure notice only runs *when the guard is true*) — that's the graded distinction to call out explicitly if asked to justify the choice.
- Feel free to swap "Search articles" or "Unsubscribe from feed" in as a fourth full spec if your team wants more coverage; the diagram already includes both as valid targets.
