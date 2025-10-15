#pragma once
#include <string>
#include <vector>

// Get an OAuth access token using Client Credentials.
// Returns token string on success; empty string on failure.
// error_out (optional) is filled with a human-readable error if something fails.
std::string spotify_fetch_access_token(const std::string& client_id,
                                       const std::string& client_secret,
                                       std::string& error_out);

// Search Spotify and return the first track's title, artist, and preview_url (may be empty).
// Returns true on success (even if preview_url is empty), false on fatal error.
// error_out is filled if false is returned.
bool spotify_search_first_preview(const std::string& token,
                                  const std::string& query,
                                  std::string& out_title,
                                  std::string& out_artist,
                                  std::string& out_preview_url,
                                  std::string& error_out);

// Download raw bytes from an HTTPS URL into out_bytes. Returns true on success.
bool http_download_bytes(const std::string& url,
                         std::vector<unsigned char>& out_bytes,
                         std::string& error_out);
