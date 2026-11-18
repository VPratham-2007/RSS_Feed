# Team ID - 40

## 20251501170	VAGHELA PRATHAM KAMLESHBHAI
## 20251501137	PRIYANSHU BHATT
## 20251501139	PUNEET RATTAN
## 20251501143	REET JAIN
## 20251501161	SHUBH KETAN PANCHAL

# Noun Analysis — RSS Feed Reader

## Scope

This analysis is based on the following three use case specifications:

1. Subscribe to Feed
2. Poll Feed for Updates
3. Monitor Feed Health

and on the implemented prototype (`rss.html` front end and `server.cpp` C++ back end), so that the surviving classes and attribute names match the code.

The purpose of noun–verb analysis is to identify candidate domain classes from nouns and candidate responsibilities from verbs. The candidates are then filtered to keep only meaningful domain concepts.

## Raw Candidate List

### Candidate Nouns

Reader, Feed, Feed URL, Subscription, Article, GUID, Title, Link, Publication Date, Content, Scheduler, Polling List, Administrator, Feed Status, Last Polled Timestamp, Consecutive Failure Count, Failure Notice, Notification Service, HTTP Request, XML/JSON, Dashboard, Account, Timestamp, Removal Log, Bookmark, Cache, Plan Type, Active Days, Reader Name, Read Flag, Error Code, Exception

### Candidate Verbs

Subscribe, Validate, Check, Store, Add, Poll, Fetch, Parse, Compare, Create, Mark Unread, Update, Increment, Send, Suspend, Monitor, Retry, Remove, Reset, Log, Convert, Cache, Display, Combine

## Filters Applied

The following four filters are applied to the candidate nouns:

1. **Redundant / Synonym** — duplicates another candidate or can be represented by an existing class.
2. **Attribute** — better represented as data stored in another class.
3. **External to System Boundary** — an actor or external system rather than an internal domain class.
4. **Too Vague / Implementation Detail** — a UI element, technical mechanism, data format, or overly general concept rather than a meaningful domain class.

## Candidate Filtering

| Candidate | Decision | Filter Applied | Reason |
|---|---|---|---|
| Reader | Survives | — | Core domain user; class `Reader` |
| Feed | Survives | — | Core source that is loaded, parsed, and monitored; class `Feed` |
| Feed URL | Discarded | Attribute | Stored as feedUrl in Feed |
| Subscription | Survives | — | Represents the reader's subscription plan (planType, activeDays); class `Subscription` |
| Article | Survives | — | Core content item extracted from a feed; class `Article` |
| GUID | Discarded | Attribute | Would be stored as guid in Article; the prototype identifies an item by its link, so guid is not stored |
| Title | Discarded | Attribute | Stored as title in Article |
| Link | Discarded | Attribute | Stored as feedUrl in Article (inherited from Feed), holding the article's link |
| Publication Date | Discarded | Attribute | Stored as pubDate in Article |
| Content | Discarded | Attribute | Stored as summary in Article |
| Scheduler | Discarded | External to System Boundary | Time-triggered system actor (not implemented in the prototype; loading is on demand) |
| Polling List | Discarded | Redundant / Synonym | Represented through feed state and polling data |
| Administrator | Discarded | External to System Boundary | Human actor outside the domain model |
| Feed Status | Discarded | Attribute | Planned as status in Feed (not stored in the prototype) |
| Last Polled Timestamp | Discarded | Attribute | Planned as lastPolledAt in Feed (not stored in the prototype) |
| Consecutive Failure Count | Discarded | Attribute | Planned as consecutiveFailureCount in Feed (not stored in the prototype) |
| Failure Notice | Survives | — | Domain object created when loading a feed fails; class `FailureNotice` with errorCode |
| Notification Service | Discarded | External to System Boundary | External system responsible for delivering notifications |
| HTTP Request | Discarded | Too Vague / Implementation Detail | Technical communication mechanism (libcurl in `Feed::updateFeed`) |
| XML/JSON | Discarded | Too Vague / Implementation Detail | Data format (parsed with pugixml in `parseXmlWithPugi`) |
| Dashboard | Discarded | Too Vague / Implementation Detail | User interface concept |
| Account | Discarded | Redundant / Synonym | Represented by Reader in this domain model |
| Timestamp | Discarded | Attribute | Represented by specific time attributes such as pubDate |
| Removal Log | Discarded | Too Vague / Implementation Detail | Audit logging is not modeled as a core domain class |
| Bookmark | Survives | — | A saved feed URL that can be converted into a Feed (`Feed(const Bookmark&)`); class `Bookmark` |
| Cache | Survives | — | Holds already loaded data so a repeated request for the same feed URL is not fetched again; template class `Cache<T>` |
| Plan Type | Discarded | Attribute | Stored as planType in Subscription (Free / Trial / Premium) |
| Active Days | Discarded | Attribute | Stored as activeDays in Subscription |
| Reader Name | Discarded | Attribute | Stored as readerName in Reader |
| Read Flag | Discarded | Attribute | Stored as isRead in ArticleStatus |
| Error Code | Discarded | Attribute | Stored as errorCode in FailureNotice |
| Exception | Discarded | Too Vague / Implementation Detail | Language mechanism (`FeedException`, `InvalidSubscriptionException`) used to signal failures; the domain concept is FailureNotice |

## Surviving Classes

Reader, Feed, Subscription, Article, ArticleStatus, FailureNotice, Bookmark, Cache

### Note on ArticleStatus

ArticleStatus represents the read/unread state of an article (attribute isRead). In the implementation it extends Article, and the web page toggles the flag with "Mark read / Mark unread".

### Note on Subscription

In the implementation a Subscription is the reader's plan: planType is derived from activeDays (Free, Trial, or Premium), and an `int` converts to a Subscription and back. A feed that a reader follows is represented by a Bookmark that converts to a Feed.

## Final Result

Domain flow: Reader → Subscription (plan); Bookmark → Feed → Article

Inheritance in the code: Feed ← Article ← ArticleStatus ← Reader (Reader also inherits from Subscription); Feed ← FailureNotice

Additional classes: ArticleStatus, FailureNotice, Bookmark, Cache
