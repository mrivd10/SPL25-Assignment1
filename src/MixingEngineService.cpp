#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(0), auto_sync(false), bpm_tolerance(0)
{
    decks[0] = nullptr;
    decks[1] = nullptr;
     std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
 std::cout << "[MixingEngineService] Cleaning up decks...." << std::endl;
 for (int i = 0; i < 2; ++i) {
        if (decks[i] != nullptr) {
            delete decks[i];
            decks[i] = nullptr;
        }
    }
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    std::cout << "\n=== Loading Track to Deck ===\n";

    PointerWrapper<AudioTrack> cloned_track = track.clone();

    if (cloned_track.get() == nullptr) {
        std::cerr << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone\n";
        return -1;
    }

    int target = 1 - active_deck;
    std::cout << "[Deck Switch] Target deck: " << target << "\n";

    if (decks[target] != nullptr) {
        delete decks[target];
        decks[target] = nullptr;
    }

    cloned_track.get()->load();
    cloned_track.get()->analyze_beatgrid();

    if (decks[active_deck] != nullptr && auto_sync) {
        if (!can_mix_tracks(cloned_track)) {
            sync_bpm(cloned_track);
        }
    }

    decks[target] = cloned_track.release();
    std::cout << "[Load Complete] '" << track.get_title()
              << "' is now loaded on deck " << target << "\n";

    if (decks[active_deck] != nullptr) {
        std::cout << "[Unload] Unloading previous deck "
                  << active_deck << " (" << decks[active_deck]->get_title() << ")\n";
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }

    active_deck = target;
    std::cout << "[Active Deck] Switched to deck " << active_deck << "\n";

    return active_deck;
}


/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if (decks[active_deck] == nullptr) {
        return false;
    }
    if (track.get() == nullptr) {
        return false;
    }
    int bpm_active = decks[active_deck]->get_bpm();
    int bpm_new = track.get()->get_bpm();
    int bpm_diff = std::abs(bpm_active - bpm_new);
    return bpm_diff <= bpm_tolerance;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
  
}

