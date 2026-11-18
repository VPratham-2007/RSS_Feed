#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <exception>
#include <map>
#include <mutex>


#include <httplib.h>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <curl/curl.h>

using namespace std;
using json = nlohmann::json;


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


template <typename T>
class Cache {
private:
    T data;

public:
    Cache() = default;
    Cache(T d) : data(d) {}

    void showCache() {
    }

    T getData() const { return data; }
};

template <typename T>
void printItem(T item) {
    (void)item;
}

class Feed;
class Reader;

class Bookmark {
public:
    string url;
    Bookmark(string u) : url(u) {}
};


class FeedException : public exception {
protected:
    string customMsg;
public:
    FeedException(string msg = "General feed exception") : customMsg(msg) {}
    const char* what() const noexcept override {
        return customMsg.c_str();
    }
};

class InvalidSubscriptionException : public FeedException {
public:
    InvalidSubscriptionException() : FeedException("Invalid subscription days") {}
    const char* what() const noexcept override {
        return "Invalid subscription days";
    }
};

void validateSubscription(int days) {
    if (days < 0) {
        throw InvalidSubscriptionException();
    }
}


class Cleanup {
public:
    ~Cleanup() {
    }
};

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
        feedUrl = url.empty() ? "https://indianexpress.com/section/city/ahmedabad/feed/" : url;
        totalFeedsCreated++;
    }

    Feed(const Bookmark& b) {
        feedUrl = b.url;
        totalFeedsCreated++;
    }

    virtual ~Feed() = default;

    static void displayFeedCount() {
    }

    static int getFeedCount() { return totalFeedsCreated; }
    string getUrl() const { return feedUrl; }

    void updateFeed() {
    }

    
    string updateFeed(int limit) {
        (void)limit;
        CURL* curl = curl_easy_init();
        if (!curl) throw FeedException("Failed to initialize libcurl");

        string xmlBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, feedUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &xmlBuffer);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; RSSReader/1.0)");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw FeedException(string("CURL error: ") + curl_easy_strerror(res));
        }
        if (httpCode >= 400 || xmlBuffer.empty()) {
            throw FeedException("Feed server returned HTTP " + to_string(httpCode));
        }

        return xmlBuffer;
    }

    virtual void displayInfo() const {
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
    is >> f.feedUrl;
    return is;
}

void operator%(const Feed& other, const Feed& another) {
    (void)other;
    (void)another;
}

class Subscription {
protected:
    string planType;
    int activeDays;

public:
    Subscription() {
        planType = "Free";
        activeDays = 0;
    }

    Subscription(string plan, int days) {
        planType = plan;
        activeDays = days;
    }

    Subscription(int days) {
        activeDays = days;
        planType = (days >= 30) ? "Premium" : (days > 0 ? "Trial" : "Free");
    }

    operator int() const {
        return activeDays;
    }

    string getPlanString() const {
        return "Subscription Plan: " + planType + " (" + to_string(activeDays) + " days remaining)";
    }

    void displayPlan() {
    }
};

class Article : public Feed {
protected:
    string title;
    string pubDate;
    string summary;

public:
    Article() : Feed() {
        title = "Untitled";
    }

    Article(string url, string t, string d = "", string s = "") : Feed(url) {
        title = t.empty() ? "Untitled" : t;
        pubDate = d;
        summary = s;
    }

    string getTitle() const { return title; }
    string getDate() const { return pubDate; }
    string getSummary() const { return summary; }

    void displayArticle() {
    }

    void displayInfo() const override {
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
        cout << "WARNING: Failed to load feed '" << feedUrl << "' (Error Code: " << errorCode << ")" << endl;
    }
};

class ArticleStatus : public Article {
protected:
    bool isRead;

public:
    ArticleStatus() : Article() {
        isRead = false;
    }

    ArticleStatus(string url, string t, bool read, string d = "", string s = "")
        : Article(url, t, d, s) {
        isRead = read;
    }

    bool getRead() const { return isRead; }

    void displayStatus() {
        displayArticle();
    }
};

class Reader : public ArticleStatus, public Subscription {
private:
    string readerName;

public:
    Reader() : ArticleStatus(), Subscription() {
        readerName = "Unknown";
    }

    Reader(string name, string url, string t, bool read, string plan, int days)
        : ArticleStatus(url, t, read), Subscription(plan, days) {
        readerName = name.empty() ? "Unknown" : name;
    }

    Reader(string name, const Subscription& sub)
        : ArticleStatus(), Subscription(sub) {
        readerName = name.empty() ? "Unknown" : name;
    }

    string getName() const { return readerName; }

    void displayReaderProfile() {
        displayPlan();
        displayStatus();
    }

    friend void checkAccess(const Reader& r, const Feed& f);
    friend ostream& operator<<(ostream& os, const Reader& r);
    friend istream& operator>>(istream& is, Reader& r);
};

ostream& operator<<(ostream& os, const Reader& r) {
    os << "\n--- Reader Profile (Stream Output) ---\n"
       << "Name: " << r.readerName << "\n"
       << "Subscription Plan: " << r.planType << "\n"
       << "Article Title: " << r.title
       << " | Source: " << r.feedUrl << "\n"
       << "Status: " << (r.isRead ? "Read" : "Unread") << "\n"
       << "--------------------------------------";
    return os;
}

istream& operator>>(istream& is, Reader& r) {
    is >> r.readerName >> r.planType >> r.feedUrl >> r.title >> r.isRead;
    return is;
}

void checkAccess(const Reader& r, const Feed& f) {
    (void)r;
    (void)f;
}


void printFeedByValue(Feed f) {
    f.displayInfo();
}

void printFeedByReference(const Feed& f) {
    f.displayInfo();
}


struct ParsedFeed {
    string feedTitle;
    vector<ArticleStatus> articles;
};

ParsedFeed parseXmlWithPugi(const string& xml, int limit) {
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_string(xml.c_str());
    if (!res) {
        throw FeedException(string("XML Parse Error: ") + res.description());
    }

    ParsedFeed out;
    
    pugi::xml_node channel = doc.child("rss").child("channel");
    if (!channel) channel = doc.child("RDF");
    if (channel) {
        out.feedTitle = channel.child_value("title");
        int count = 0;
        for (pugi::xml_node item : channel.children("item")) {
            if (count++ >= limit) break;
            string title = item.child_value("title");
            string link = item.child_value("link");
            string pubDate = item.child_value("pubDate");
            if (pubDate.empty()) pubDate = item.child_value("dc:date");
            string desc = item.child_value("description");
            if (desc.empty()) desc = item.child_value("content:encoded");

            out.articles.emplace_back(link, title, false, pubDate, desc);
        }
        return out;
    }

    
    pugi::xml_node atomFeed = doc.child("feed");
    if (atomFeed) {
        out.feedTitle = atomFeed.child_value("title");
        int count = 0;
        for (pugi::xml_node entry : atomFeed.children("entry")) {
            if (count++ >= limit) break;
            string title = entry.child_value("title");
            string link;
            for (pugi::xml_node l : entry.children("link")) {
                string rel = l.attribute("rel").value();
                if (rel.empty() || rel == "alternate") {
                    link = l.attribute("href").value();
                    break;
                }
            }
            if (link.empty()) link = entry.child("link").attribute("href").value();

            string pubDate = entry.child_value("published");
            if (pubDate.empty()) pubDate = entry.child_value("updated");

            string desc = entry.child_value("summary");
            if (desc.empty()) desc = entry.child_value("content");

            out.articles.emplace_back(link, title, false, pubDate, desc);
        }
        return out;
    }

    throw FeedException("Document is neither a valid RSS nor Atom feed.");
}


int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    httplib::Server svr;
    mutex stateMutex;

    map<string, ParsedFeed> memoryCache;
    Reader activeReader("papon", "https://indianexpress.com/section/city/ahmedabad/feed/", "Welcome", false, "Premium", 45);

    
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        ifstream file("rss.html");
        if (!file.is_open()) {
            res.status = 404;
            res.set_content("rss.html not found in current working directory.", "text/plain");
            return;
        }
        stringstream buffer;
        buffer << file.rdbuf();
        res.set_content(buffer.str(), "text/html; charset=utf-8");
    });

    
    svr.Get("/api/feed", [&](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(stateMutex);

        string feedUrl = req.has_param("url") ? req.get_param_value("url") : "";
        int limit = req.has_param("limit") ? max(1, min(50, stoi(req.get_param_value("limit")))) : 10;
        bool fromBm = req.has_param("bookmark") && req.get_param_value("bookmark") == "1";

        json payload;
        int httpStatus = 200;

        {
            Cleanup cleanup; 
            

            try {
                
                Feed currentFeed = fromBm ? Feed(Bookmark(feedUrl)) : Feed(feedUrl);

                
                checkAccess(activeReader, currentFeed);

                
                printItem(currentFeed);

                ParsedFeed parsed;
                string actualUrl = currentFeed.getUrl();

                if (memoryCache.count(actualUrl) && (int)memoryCache[actualUrl].articles.size() >= limit) {
                    Cache<string> urlCache(actualUrl + " (" + to_string(memoryCache[actualUrl].articles.size()) + " cached items)");
                    urlCache.showCache();
                    parsed = memoryCache[actualUrl];
                } else {
                    string xml = currentFeed.updateFeed(limit);
                    parsed = parseXmlWithPugi(xml, limit);
                    if (parsed.articles.empty()) {
                        throw FeedException("No articles found inside feed");
                    }
                    memoryCache[actualUrl] = parsed;
                }

                
                if (!parsed.articles.empty()) {
                    printFeedByValue(parsed.articles[0]);   
                    
                    printFeedByReference(parsed.articles[0]); 
                }

                Feed::displayFeedCount();

                json itemsArray = json::array();
                int count = min(limit, (int)parsed.articles.size());
                for (int i = 0; i < count; ++i) {
                    const auto& a = parsed.articles[i];
                    itemsArray.push_back({
                        {"link", a.getUrl()},
                        {"title", a.getTitle()},
                        {"pubDate", a.getDate()},
                        {"description", a.getSummary()},
                        {"isRead", a.getRead()}
                    });
                }

                payload["status"] = "ok";
                payload["feed"] = {{"title", parsed.feedTitle}, {"url", actualUrl}};
                payload["items"] = itemsArray;
                payload["totalFeedsCreated"] = Feed::getFeedCount();

            } catch (const InvalidSubscriptionException& e) {
                httpStatus = 400;
                payload["status"] = "error";
                payload["message"] = e.what();
            } catch (const FeedException& e) {
                httpStatus = 502;
                FailureNotice notice(feedUrl, httpStatus);
                notice.showError();
                payload["status"] = "error";
                payload["message"] = e.what();
            } catch (const exception& e) {
                httpStatus = 500;
                payload["status"] = "error";
                payload["message"] = e.what();
            }
        } 
        
        res.status = httpStatus;
        res.set_content(payload.dump(), "application/json; charset=utf-8");
    });

    
    svr.Post("/api/reader", [&](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(stateMutex);

        json payload;
        int httpStatus = 200;

        {
            Cleanup cleanup; 
            
            try {
                auto body = json::parse(req.body.empty() ? "{}" : req.body);
                string name = body.value("name", "Alice");
                int days = body.value("days", 0);

                validateSubscription(days);

              
                
                Subscription sub = days;
                sub.displayPlan();

                
                int daysLeft = sub;
                printItem(daysLeft);

                activeReader = Reader(name, sub);

                payload["status"] = "ok";
                payload["readerName"] = activeReader.getName();
                payload["plan"] = sub.getPlanString();

            } catch (const InvalidSubscriptionException& e) {
                httpStatus = 400;
                payload["status"] = "error";
                payload["message"] = e.what();
            } catch (const FeedException& e) {
                httpStatus = 400;
                payload["status"] = "error";
                payload["message"] = e.what();
            } catch (const exception& e) {
                httpStatus = 400;
                payload["status"] = "error";
                payload["message"] = "Invalid JSON body";
            }
        } 

        res.status = httpStatus;
        res.set_content(payload.dump(), "application/json; charset=utf-8");
    });

    svr.Get("/api/combine", [&](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(stateMutex);

        string url1 = req.has_param("url1") ? req.get_param_value("url1") : "";
        string url2 = req.has_param("url2") ? req.get_param_value("url2") : "";

        Feed f1(url1), f2(url2);
        f1 % f2; 
        
        json payload = {
            {"status", "ok"},
            {"totalFeedsCreated", Feed::getFeedCount()}
        };
        res.set_content(payload.dump(), "application/json; charset=utf-8");
    });

    cout << "=== C++ RSS Feed Reader Server running at http://localhost:3000 ===" << endl;
    svr.listen("0.0.0.0", 3000);

    curl_global_cleanup();
    return 0;
}