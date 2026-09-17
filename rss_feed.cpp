#include <iostream>
#include <string>
using namespace std;

class Feed;
class Reader;

class Feed {
private:
    static int totalFeedsCreated; 

protected:
    string feedUrl;

public:
    Feed() {
        feedUrl = "https://indianexpress.com/section/city/ahmedabad/feed/";
        totalFeedsCreated++; 
    }
    
    Feed(string url) {
        feedUrl = url;
        totalFeedsCreated++; 
    }

    static void displayFeedCount() {
        cout << "[Static Info] Total Feed objects currently in memory: " 
             << totalFeedsCreated << endl;
    }

    void updateFeed() {
        cout << "Updating all articles from: " << feedUrl << endl;
    }
    
    void updateFeed(int limit) {
        cout << "Updating top " << limit << " articles from: " << feedUrl << endl;
    }


    friend void operator%(const Feed& other, const Feed& another);
    friend void checkAccess(const Reader& r, const Feed& f);
    friend ostream& operator<<(ostream& os, const Feed& f); 
    friend istream& operator>>(istream& is, Feed& f);
};

int Feed::totalFeedsCreated = 0;

ostream& operator<<(ostream& os, const Feed& f) {
    os << "[Feed Object] URL: " << f.feedUrl;
    return os;
}

istream& operator>>(istream& is, Feed& f) {
    cout << "Enter Feed URL: ";
    is >> f.feedUrl;
    return is;
}

void operator%(const Feed& other, const Feed& another){
    cout << "Fetching news from both feeds: " << another.feedUrl << " and " << other.feedUrl << endl;
}

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

    void displayInfo() const override {
        cout << "[Derived Article] Title: " << title << " | Source: " << feedUrl << endl;
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
    Reader() : ArticleStatus(), Subscription() {
        readerName = "Unknown";
    }

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
    friend ostream& operator<<(ostream& os, const Reader& r);
    friend istream& operator>>(istream& is, Reader& r);
};

ostream& operator<<(ostream& os, const Reader& r) {
    os << "\n--- Reader Profile (Stream Output) ---\n"
       << "Name: " << r.readerName << "\n"
       << "Subscription Plan: " << r.planType << "\n"
       << "Article Title: " << r.title << " | Source: " << r.feedUrl << "\n"
       << "Status: " << (r.isRead ? "Read" : "Unread") << "\n"
       << "--------------------------------------";
    return os;
}

istream& operator>>(istream& is, Reader& r) {
    cout << "Enter Reader Name: ";
    is >> r.readerName;
    
    cout << "Enter Subscription Plan: ";
    is >> r.planType;
    
    cout << "Enter Feed URL: ";
    is >> r.feedUrl;
    
    cout << "Enter Article Title (one word): ";
    is >> r.title;
    
    cout << "Has it been read? (1 for Yes, 0 for No): ";
    is >> r.isRead;
    
    return is;
}

void checkAccess(const Reader& r, const Feed& f) {
    cout << "[System Check]: Reader '" << r.readerName 
         << "' is trying to ping external feed URL: '" << f.feedUrl << "'" << endl;
}

void printFeedByValue(Feed f) {
    f.displayInfo();
}

void printFeedByReference(const Feed& f) {
    f.displayInfo();
}

int main() {
    cout << "=== Start of Program ===" << endl;
    Feed::displayFeedCount(); 

    cout << "\n=== Demonstrating Object Slicing ===" << endl;
    Article myArticle("http://tech-news.com/rss", "Understanding C++ Slicing");
    Feed::displayFeedCount(); 
    
    cout << "\n1. Passing by Value (SLICING OCCURS):" << endl;
    printFeedByValue(myArticle); 

    cout << "After Pass by Value:" << endl;
    Feed::displayFeedCount(); 
    cout << "\n2. Passing by Reference (NO SLICING):" << endl;
    printFeedByReference(myArticle);
    
    cout << "\n====================================\n" << endl;

    Feed techFeed("http://tech-news.com/rss");
    cout << "Printing techFeed directly: " << techFeed << endl;
    Feed::displayFeedCount(); 
    Reader myReader("Alice", "http://cpp-tutorials.com/rss", "Learn OOP", true, "Premium");
    Feed::displayFeedCount();
    return 0;
}