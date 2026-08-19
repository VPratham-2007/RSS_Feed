# Team ID - 40

## 20251501170	VAGHELA PRATHAM KAMLESHBHAI
## 20251501137	PRIYANSHU BHATT
## 20251501139	PUNEET RATTAN
## 20251501143	REET JAIN
## 20251501161	SHUBH KETAN PANCHAL

# CRC Cards — RSS Feed Reader

## 1. Reader

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Reader** | Subscribe to feeds; unsubscribe from feeds; view subscribed articles; mark articles as read or unread | Subscription, ArticleStatus, Article |

## 2. Feed

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Feed** | Store feed information and URL; maintain feed status; track last polling time; track consecutive failures; provide articles belonging to the feed; record successful or failed polling state | Subscription, Article, FailureNotice |

## 3. Subscription

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Subscription** | Connect a Reader with a Feed; record subscription date; support creation of a subscription; support removal of a subscription; help prevent duplicate subscriptions | Reader, Feed |

## 4. Article

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Article** | Store article GUID, title, link, publication date, and content; belong to a Feed; provide article information for readers | Feed, ArticleStatus |

## 5. ArticleStatus

| Class | Responsibilities | Collaborators |
|---|---|---|
| **ArticleStatus** | Track whether a specific Reader has read a specific Article; mark an article as read; mark an article as unread; store the read time when applicable | Reader, Article |

## 6. FailureNotice

| Class | Responsibilities | Collaborators |
|---|---|---|
| **FailureNotice** | Record a feed failure; store the failure reason; record notice creation time; track whether the notice has been sent | Feed |

## Summary of Collaborations

- A **Reader** collaborates with **Subscription** to manage followed feeds.
- A **Subscription** connects a **Reader** and a **Feed**.
- A **Feed** produces many **Article** objects.
- **ArticleStatus** connects a **Reader** with an **Article** to maintain individual read/unread state.
- A **Feed** may generate **FailureNotice** objects when polling fails.
