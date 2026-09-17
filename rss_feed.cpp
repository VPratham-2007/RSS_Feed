#include <iostream>
#include <string>
using namespace std;

class Feed;
class Reader;

class Feed {
protected:
    string feedUrl;

public:
    Feed() {
        feedUrl = "https://indianexpress.com/section/city/ahmedabad/feed/"; //Instead of Ahmedabad user can give input
    }
    
    Feed(string url) {
        feedUrl = url;
    }

    void updateFeed() {
        cout << "Updating all articles from: " << feedUrl << endl;
        // New news will be fetched from the feed URL and stored in the database.
    }

    void updateFeed(int count) {
        cout << "Updating " << count << " latest articles from: " << feedUrl << endl;
    }

    void operator%(const Feed& other) {
        cout << "Fetching news from both feeds: " << feedUrl << " AND " << other.feedUrl << endl;
        // Fetches news from both feeds and combines them.
    }

    friend void checkAccess(const Reader& r, const Feed& f);
};
    
class Subscription {
protected:
    string planType;

public:
    Subscription() { planType = "Free"; }
    Subscription(string plan) { planType = plan; }
    
    void displayPlan() {
        cout << "Subscription Plan: " << planType << endl;
    }
};

class Article : public Feed {
protected:
    string title;

public:
    Article() : Feed() { title = "Untitled"; }
    Article(string url, string t) : Feed(url) { title = t; }
    
    void displayArticle() {
        cout << "Article Title: " << title << " | Source: " << feedUrl << endl;
        cout << "Fetching content from: " << feedUrl << endl;
    }
};

class FailureNotice : public Feed {
private:
    int errorCode;

public:
    FailureNotice(string url, int code) : Feed(url) {
        errorCode = code;
    }
    
    void showError() {
        cout << "WARNING: Failed to load feed '" << feedUrl 
             << "' (Error Code: " << errorCode << ")" << endl;
    }
};

class ArticleStatus : public Article {
protected:
    bool isRead;

public:
    ArticleStatus() : Article() { isRead = false; }
    ArticleStatus(string url, string t, bool read) : Article(url, t) {
        isRead = read;
    }
    
    void displayStatus() {
        displayArticle();
        cout << "Status: " << (isRead ? "Read" : "Unread") << endl;
    }
};

class Reader : public ArticleStatus, public Subscription {
private:
    string readerName;

public:
    Reader(string name, string url, string t, bool read, string plan) 
        : ArticleStatus(url, t, read), Subscription(plan) {
        readerName = name;
    }

    void displayReaderProfile() {
        cout << "\n--- Reader Profile ---" << endl;
        cout << "Name: " << readerName << endl;
        displayPlan();
        displayStatus();
        cout << "----------------------\n" << endl;
    }
    
    friend void checkAccess(const Reader& r, const Feed& f);
};

// This function can access private/protected members of BOTH Reader and Feed
void checkAccess(const Reader& r, const Feed& f) {
    cout << "[System Check]: Reader '" << r.readerName 
         << "' is trying to ping external feed URL: '" << f.feedUrl << "'" << endl;
}

int main() {
    // Function Overloading using Feed
    Feed techFeed("http://tech-news.com/rss");
    techFeed.updateFeed();      
    techFeed.updateFeed(5);

    // FailureNotice 
    cout << endl;
    FailureNotice error("http://broken-link.com/rss", 404);
    error.showError();

    // Hybrid Inheritance Reader Inherits from ArticleStatus and Subscription
    Reader myReader("Alice", "http://cpp-tutorials.com/rss", "Learn OOP", true, "Premium");
    myReader.displayReaderProfile();

    // Friend Function
    Feed newFeed("http://new-blog.com/rss");
    checkAccess(myReader, newFeed);

    // Operator Overloading
    cout << endl;
    Feed feedA("http://cpp-tutorials.com/rss");
    Feed feedB("http://different-site.com/rss");
    feedA % feedB;

    return 0;
}