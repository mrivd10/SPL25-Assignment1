#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>

DJLibraryService::~DJLibraryService() {
    for (AudioTrack* track : library) {
        delete track;   
    }
    library.clear();
}

DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    for (int i = 0; i < (int)library_tracks.size(); i++) {
        const SessionConfig::TrackInfo& info = library_tracks[i];
        AudioTrack* track = nullptr;

        if (info.type == "MP3") {
            track = new MP3Track(
                info.title,
                info.artists,
                info.duration_seconds,
                info.bpm,
                info.extra_param1,
                info.extra_param2
            );
        }
        else if (info.type == "WAV") {
            track = new WAVTrack(
                info.title,
                info.artists,
                info.duration_seconds,
                info.bpm,
                info.extra_param1,
                info.extra_param2
            );
        }

        if (track) {
            library.push_back(track);
            std::cout << "library_track_"
            << i << "="
            << info.type << ","
            << info.title << ",{"
            << info.artists[0] << "},"
            << info.duration_seconds << ","
            << info.bpm << ","
            << info.extra_param1 << ","
            << info.extra_param2 << std::endl;
        }
    }

    std::cout << "[INFO] Track library built: " << library.size() << " tracks loaded" << std::endl;
}


/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    AudioTrack* track = playlist.find_track(track_title);
    return track;
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    // For now, add a placeholder to fix the linker error
    playlist = Playlist(playlist_name);
    for (int index : track_indices) {
        if (index < 1 || index > (int)library.size()) {
            std::cerr << "[WARNING] Invalid track index: " << index << std::endl;
            continue;
        }
        
        // get the cloned track
        AudioTrack* track = library[index - 1]->clone().release();
        if (!track) {
            std::cerr << "[ERROR] Failed to clone track at index: " << index << std::endl;
            continue;
        }
        track->load();
        track->analyze_beatgrid();
        // add to playlist
        playlist.add_track(track);
        std::cout << "Added '" << track->get_title() << "' to playlist '" << playlist_name << "'" << std::endl;
    }
}

/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;
    for (AudioTrack* track : playlist.getTracks()) {
        titles.push_back(track->get_title());
    }
    return titles;
}
