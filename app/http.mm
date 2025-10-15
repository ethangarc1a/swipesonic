#import <Foundation/Foundation.h>
#include <string>
#include <vector>
#include "spotify_bridge.hpp"

static NSData* SyncRequest(NSURLRequest* req, NSInteger& httpStatus, NSString*& outErrStr) {
    __block NSData* result = nil;
    __block NSURLResponse* resp = nil;
    __block NSError* err = nil;

    dispatch_semaphore_t sem = dispatch_semaphore_create(0);
    [[[NSURLSession sharedSession] dataTaskWithRequest:req
                                     completionHandler:^(NSData* data, NSURLResponse* response, NSError* error) {
        result = data;
        resp = response;
        err = error;
        dispatch_semaphore_signal(sem);
    }] resume];
    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);

    httpStatus = 0;
    if ([resp isKindOfClass:[NSHTTPURLResponse class]]) {
        httpStatus = [(NSHTTPURLResponse*)resp statusCode];
    }
    if (err) outErrStr = [err localizedDescription];
    return result;
}

static std::string URLEncode(const std::string& s) {
    NSString* ns = [NSString stringWithUTF8String:s.c_str()];
    NSString* enc = [ns stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    return std::string([enc UTF8String]);
}

std::string spotify_fetch_access_token(const std::string& client_id,
                                       const std::string& client_secret,
                                       std::string& error_out) {
    // POST https://accounts.spotify.com/api/token
    // grant_type=client_credentials, Authorization: Basic base64(id:secret)
    NSString* cid = [NSString stringWithUTF8String:client_id.c_str()];
    NSString* csec = [NSString stringWithUTF8String:client_secret.c_str()];
    NSString* combo = [NSString stringWithFormat:@"%@:%@", cid, csec];
    NSData* comboData = [combo dataUsingEncoding:NSUTF8StringEncoding];
    NSString* b64 = [comboData base64EncodedStringWithOptions:0];

    NSURL* url = [NSURL URLWithString:@"https://accounts.spotify.com/api/token"];
    NSMutableURLRequest* req = [NSMutableURLRequest requestWithURL:url];
    req.HTTPMethod = @"POST";
    [req setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    [req setValue:[NSString stringWithFormat:@"Basic %@", b64] forHTTPHeaderField:@"Authorization"];
    req.HTTPBody = [@"grant_type=client_credentials" dataUsingEncoding:NSUTF8StringEncoding];

    NSInteger status = 0; NSString* errStr = nil;
    NSData* data = SyncRequest(req, status, errStr);

    if (!data || status < 200 || status >= 300) {
        error_out = errStr ? std::string([errStr UTF8String]) : ("HTTP " + std::to_string((int)status));
        return {};
    }

    NSError* jsonErr = nil;
    NSDictionary* json = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonErr];
    if (jsonErr || ![json isKindOfClass:[NSDictionary class]]) {
        error_out = jsonErr ? std::string([[jsonErr localizedDescription] UTF8String]) : "Bad JSON";
        return {};
    }
    NSString* token = json[@"access_token"];
    if (!token) { error_out = "No access_token"; return {}; }
    return std::string([token UTF8String]);
}

bool spotify_search_first_preview(const std::string& token,
                                  const std::string& query,
                                  std::string& out_title,
                                  std::string& out_artist,
                                  std::string& out_preview_url,
                                  std::string& error_out) {
    NSString* auth = [NSString stringWithFormat:@"Bearer %s", token.c_str()];
    std::string qURL = "https://api.spotify.com/v1/search?type=track&limit=1&q=" + URLEncode(query);
    NSURL* url = [NSURL URLWithString:[NSString stringWithUTF8String:qURL.c_str()]];
    NSMutableURLRequest* req = [NSMutableURLRequest requestWithURL:url];
    [req setValue:auth forHTTPHeaderField:@"Authorization"];

    NSInteger status = 0; NSString* errStr = nil;
    NSData* data = SyncRequest(req, status, errStr);
    if (!data || status < 200 || status >= 300) {
        error_out = errStr ? std::string([errStr UTF8String]) : ("HTTP " + std::to_string((int)status));
        return false;
    }
    NSError* jsonErr = nil;
    NSDictionary* json = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonErr];
    if (jsonErr || ![json isKindOfClass:[NSDictionary class]]) {
        error_out = jsonErr ? std::string([[jsonErr localizedDescription] UTF8String]) : "Bad JSON";
        return false;
    }
    NSDictionary* tracks = json[@"tracks"];
    NSArray* items = tracks ? tracks[@"items"] : nil;
    if (![items isKindOfClass:[NSArray class]] || items.count == 0) {
        error_out = "No tracks found";
        return false;
    }
    NSDictionary* item = items[0];
    NSString* name = item[@"name"];
    NSArray* artists = item[@"artists"];
    NSString* artistName = nil;
    if ([artists isKindOfClass:[NSArray class]] && artists.count > 0) {
        artistName = [artists[0] valueForKey:@"name"];
    }
    id prev = item[@"preview_url"]; // may be NSNull
    NSString* previewURL = ([prev isKindOfClass:[NSNull class]] || prev == nil) ? nil : (NSString*)prev;

    out_title = name ? std::string([name UTF8String]) : std::string("Unknown Title");
    out_artist = artistName ? std::string([artistName UTF8String]) : std::string("Unknown Artist");
    out_preview_url = previewURL ? std::string([previewURL UTF8String]) : std::string(); // empty if none
    return true;
}

bool http_download_bytes(const std::string& url,
                         std::vector<unsigned char>& out_bytes,
                         std::string& error_out) {
    NSURL* nurl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
    NSMutableURLRequest* req = [NSMutableURLRequest requestWithURL:nurl];
    NSInteger status = 0; NSString* errStr = nil;
    NSData* data = SyncRequest(req, status, errStr);
    if (!data || status < 200 || status >= 300) {
        error_out = errStr ? std::string([errStr UTF8String]) : ("HTTP " + std::to_string((int)status));
        return false;
    }
    out_bytes.resize([data length]);
    memcpy(out_bytes.data(), [data bytes], [data length]);
    return true;
}
