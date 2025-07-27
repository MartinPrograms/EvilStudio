#include "PianoRollWindow.h"

#include "audio/piano.h"

namespace ui {
namespace Windows {
    void PianoRollWindow::OnRender(mu_Context *ctx) {
        int cw[] = {-1};
        mu_layout_row(ctx, 1, cw, 0);

        if (!backend->sequencer_state.patterns.empty()) {
            mu_popup_selector(
                ctx,
                "Select Pattern",
                "Pattern",
                backend->sequencer_state.patterns,
                [](const audio::Sequencing::Pattern& p) {  // Convert Pattern to string (options)
                    return quick_format("Pattern {}", p.id);
                },
                [](long id) {  // Convert current ID to string
                    return quick_format("Pattern {}", id);
                },
                [](const audio::Sequencing::Pattern& p) -> long {  // Extract ID from Pattern
                    return p.id;
                },
                selected_pattern
            );
        }
        else {
            mu_label(ctx, "No patterns available.");
        }

        // Add a remove/add pattern button
        int button_width = (mu_get_current_container(ctx)->body.w - ctx->style->padding) / 2;
        int button_cw[] = {button_width, -1};
        mu_layout_row(ctx, 2, button_cw, 0);

        if (mu_button(ctx, "Add Pattern")) {
            audio::Sequencing::Pattern new_pattern;
            new_pattern.id = backend->sequencer_state.id_counter++;
            new_pattern.note_sequences.emplace_back();
            backend->sequencer_state.patterns.push_back(new_pattern);
            selected_pattern = new_pattern.id;
        }

        if (mu_button(ctx, "Remove Pattern")) {
            // Find the pattern with the selected ID and remove it
            auto it = std::remove_if(backend->sequencer_state.patterns.begin(),
                                     backend->sequencer_state.patterns.end(),
                                     [this](const audio::Sequencing::Pattern& p) {
                                         return p.id == selected_pattern;
                                     });

            // If we found a pattern to remove, erase it
            if (it != backend->sequencer_state.patterns.end()) {
                backend->sequencer_state.patterns.erase(it, backend->sequencer_state.patterns.end());
                selected_pattern = -1; // Reset selection
            } else {
                mu_label(ctx, "Pattern not found.");
            }
        }

        mu_layout_row(ctx, 1, cw, 0);

        if (generators_window->selected_generator == -1) {
            mu_label(ctx, "No generator selected.");
            return;
        }

        mu_easy_popup(ctx, "Note Sequence Failed To Add",
                      "Failed to add note sequence. Please select a pattern first.");

        mu_easy_popup(ctx, "Note Sequence Already Exists",
                      "Note sequence already exists for the selected generator in this pattern.");

        mu_easy_popup(ctx, "Note Sequence Added Successfully",
                    "Note sequence added successfully.");

        // Note sequence button (add note sequence)
        if (mu_button(ctx, "Add Note Sequence")) {
            if (selected_pattern != -1) {
                // Find the pattern with the selected ID
                auto it = std::find_if(backend->sequencer_state.patterns.begin(),
                                       backend->sequencer_state.patterns.end(),
                                       [this](const audio::Sequencing::Pattern& p) {
                                           return p.id == selected_pattern;
                                       });

                if (it != backend->sequencer_state.patterns.end()) {
                    // Add a new note sequence to the found pattern if it does not already exist
                    for (auto& seq : it->note_sequences) {
                        if (seq.generator == backend->generators[generators_window->selected_generator]) {
                            mu_open_popup(ctx, "Note Sequence Already Exists");
                            return;
                        }
                    }
                    audio::Sequencing::NoteSequence new_sequence;
                    new_sequence.generator = backend->generators[generators_window->selected_generator];

                    new_sequence.notes.push_back({60, 127, 0, 0, 22050, false});

                    it->note_sequences.push_back(new_sequence);
                    mu_open_popup(ctx, "Note Sequence Added Successfully");
                } else {
                    mu_open_popup(ctx, "Note Sequence Failed To Add");
                }
            } else {
                mu_open_popup(ctx, "Note Sequence Failed To Add");
            }
        }

        if (selected_pattern != -1) {
            auto pat_it = std::find_if(backend->sequencer_state.patterns.begin(),
                                       backend->sequencer_state.patterns.end(),
                                       [this](const audio::Sequencing::Pattern& p) {
                                           return p.id == selected_pattern;
                                       });

            if (pat_it != backend->sequencer_state.patterns.end()) {
                auto& pattern = *pat_it;
                auto& generator = backend->generators[generators_window->selected_generator];

                auto seq_it = std::find_if(pattern.note_sequences.begin(),
                                           pattern.note_sequences.end(),
                                           [&generator](const audio::Sequencing::NoteSequence& ns) {
                                               return ns.generator == generator;
                                           });

                if (seq_it != pattern.note_sequences.end()) {
                    RenderPianoRoll(ctx, *seq_it);
                }
            }
        }
    }

    void PianoRollWindow::RenderPianoRoll(mu_Context* ctx, const audio::Sequencing::NoteSequence& sequence) {
        int cw[] = {-1};
        mu_layout_row(ctx, 1, cw, 0);

        if (sequence.notes.empty()) {
            mu_label(ctx, "No notes in this sequence.");
            return;
        }

        // Render the piano roll
        int note_height = 20; // Height of each note row
        int note_width = 50; // Width of each note column
        int num_notes = 128; // Total number of MIDI notes

        for (int i = 0; i < num_notes; ++i) {
            mu_Rect note_rect = mu_rect(0, i * note_height, note_width, note_height);
            mu_draw_rect(ctx, note_rect, mu_color(200, 200, 200, 255)); // Draw background for each note row
        }
        for (const auto& note : sequence.notes) {
            int note_x = note.play_time / 44100 * note_width; // Convert sample to pixel position
            int note_y = (127 - note.note_number) * note_height; // Invert pitch for Y position
            int note_w = (note.stop_time - note.play_time) / 44100 * note_width; // Width based on duration

            mu_Rect rect = mu_rect(note_x, note_y, note_w, note_height);
            mu_draw_rect(ctx, rect, mu_color(0, 255, 0, 255)); // Draw the note rectangle
        }
    }
} // Windows
} // ui