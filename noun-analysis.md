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

The purpose of noun–verb analysis is to identify candidate domain classes from nouns and candidate responsibilities from verbs. The candidates are then filtered to keep only meaningful domain concepts.

## Raw Candidate List

### Candidate Nouns

Reader, Feed, Feed URL, Subscription, Article, GUID, Title, Link, Publication Date, Content, Scheduler, Polling List, Administrator, Feed Status, Last Polled Timestamp, Consecutive Failure Count, Failure Notice, Notification Service, HTTP Request, XML/JSON, Dashboard, Account, Timestamp, Removal Log

### Candidate Verbs

Subscribe, Validate, Check, Store, Add, Poll, Fetch, Parse, Compare, Create, Mark Unread, Update, Increment, Send, Suspend, Monitor, Retry, Remove, Reset, Log

## Filters Applied

The following four filters are applied to the candidate nouns:

1. **Redundant / Synonym** — duplicates another candidate or can be represented by an existing class.
2. **Attribute** — better represented as data stored in another class.
3. **External to System Boundary** — an actor or external system rather than an internal domain class.
4. **Too Vague / Implementation Detail** — a UI element, technical mechanism, data format, or overly general concept rather than a meaningful domain class.

## Candidate Filtering

| Candidate | Decision | Filter Applied | Reason |
|---|---|---|---|
| Reader | Survives | — | Core domain user |
| Feed | Survives | — | Core source that is subscribed to, polled, and monitored |
| Feed URL | Discarded | Attribute | Stored as feedUrl in Feed |
| Subscription | Survives | — | Represents the relationship between a Reader and a Feed |
| Article | Survives | — | Core content item extracted from a feed |
| GUID | Discarded | Attribute | Stored as guid in Article |
| Title | Discarded | Attribute | Stored as title in Article |
| Link | Discarded | Attribute | Stored as link in Article |
| Publication Date | Discarded | Attribute | Stored as publishedDate in Article |
| Content | Discarded | Attribute | Stored as content in Article |
| Scheduler | Discarded | External to System Boundary | Time-triggered system actor |
| Polling List | Discarded | Redundant / Synonym | Represented through feed state and polling data |
| Administrator | Discarded | External to System Boundary | Human actor outside the domain model |
| Feed Status | Discarded | Attribute | Stored as status in Feed |
| Last Polled Timestamp | Discarded | Attribute | Stored as lastPolledAt in Feed |
| Consecutive Failure Count | Discarded | Attribute | Stored as consecutiveFailureCount in Feed |
| Failure Notice | Survives | — | Domain object created when polling fails |
| Notification Service | Discarded | External to System Boundary | External system responsible for delivering notifications |
| HTTP Request | Discarded | Too Vague / Implementation Detail | Technical communication mechanism |
| XML/JSON | Discarded | Too Vague / Implementation Detail | Data format |
| Dashboard | Discarded | Too Vague / Implementation Detail | User interface concept |
| Account | Discarded | Redundant / Synonym | Represented by Reader in this domain model |
| Timestamp | Discarded | Attribute | Represented by specific time attributes |
| Removal Log | Discarded | Too Vague / Implementation Detail | Audit logging is not modeled as a core domain class |

## Surviving Classes

Reader, Feed, Subscription, Article, ArticleStatus, FailureNotice

### Note on ArticleStatus

ArticleStatus represents the read/unread state of an article for a specific reader. This state belongs to the Reader–Article relationship rather than globally to the Article.

## Final Result

Reader → Subscription → Feed → Article

Additional classes: ArticleStatus, FailureNotice
