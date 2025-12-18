#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * Load track into cache and return cache operation result:
 * 1 = hit, 0 = miss, -1 = miss with eviction
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    const std::string& title = track.get_title();

    if (cache.contains(title)) {
        cache.get(title);
        return 1;
    }

    PointerWrapper<AudioTrack> cloned_track = track.clone();

    if (cloned_track.get() == nullptr) {
        std::cerr << "[ERROR] Track: \"" << title << "\" failed to clone" << std::endl;
        return 0;
    }

    cloned_track.get()->load();
    cloned_track.get()->analyze_beatgrid();

    bool eviction = cache.put(std::move(cloned_track));

    if (eviction) {
        return -1;
    }

    return 0;
}


void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * Retrieve a track from cache by title
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    return cache.get(track_title);
}
