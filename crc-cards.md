# Team ID - 40

## 20251501170	VAGHELA PRATHAM KAMLESHBHAI
## 20251501137	PRIYANSHU BHATT
## 20251501139	PUNEET RATTAN
## 20251501143	REET JAIN
## 20251501161	SHUBH KETAN PANCHAL

# CRC Cards — RSS Feed Reader

The cards match the classes in `server.cpp`. "Inherits from" shows the base classes used in the code.

## 1. Reader

Inherits from: ArticleStatus, Subscription

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Reader** | Store the reader's name; hold the reader's plan (inherited from Subscription); hold the current article and read/unread state (inherited from ArticleStatus); display the reader profile | Subscription, ArticleStatus, Feed |

## 2. Feed

Inherits from: none (base class)

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Feed** | Store the feed URL; count the Feed objects created (static counter); be created from a URL, from the default URL, or from a Bookmark; download the feed XML for a given article limit; display feed information (overridden by Article); signal a failed load with a FeedException | Bookmark, Article, FailureNotice, Cache |

## 3. Subscription

Inherits from: none (base class)

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Subscription** | Store plan type and active days; derive the plan (Free, Trial, Premium) from the days; convert an int into a Subscription and a Subscription back into an int; provide the plan text for display; reject negative days (InvalidSubscriptionException) | Reader |

## 4. Article

Inherits from: Feed

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Article** | Store title, publication date, and summary; keep the article link as its URL (inherited feedUrl); display article information | Feed, ArticleStatus |

## 5. ArticleStatus

Inherits from: Article

| Class | Responsibilities | Collaborators |
|---|---|---|
| **ArticleStatus** | Track whether an article is read; report the read state; display the article together with its Read/Unread status | Article, Reader |

## 6. FailureNotice

Inherits from: Feed

| Class | Responsibilities | Collaborators |
|---|---|---|
| **FailureNotice** | Record the URL of the feed that failed; store the error code; display a warning message for the failure | Feed |

## 7. Bookmark

Inherits from: none

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Bookmark** | Store the URL of a saved feed; be converted into a Feed when a saved feed is opened | Feed |

## 8. Cache

Inherits from: none (template class `Cache<T>`)

| Class | Responsibilities | Collaborators |
|---|---|---|
| **Cache** | Hold data of any type; show the cached item; return the cached data | Feed |

## Summary of Collaborations

- A **Reader** is an **ArticleStatus** and a **Subscription**; through ArticleStatus it is also an **Article** and a **Feed**.
- A **Bookmark** is converted into a **Feed** when a saved feed is opened.
- A **Feed** is loaded and parsed into **ArticleStatus** objects, one for each feed item.
- An **Article** is a **Feed**; an **ArticleStatus** is an **Article**.
- A **FailureNotice** is created (and is itself a **Feed**) when loading a feed fails.
- A **Cache** reports the items already loaded for a feed URL.
