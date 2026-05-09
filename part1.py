import os
import numpy as np
import librosa


DATASET_PATH = "recordings"
WORDS = ["on", "off", "start", "stop", "left", "right", "up", "down"]

SAMPLE_RATE = 8000
FRAME_SIZE = 200      # ~25 ms
HOP_SIZE = 200        # no overlap

OUTPUT_HEADER = "templates.h"


# ==========================
# RMS FUNCTION (ADDED)
# ==========================
def compute_rms(frame):
    return np.sqrt(np.mean(frame ** 2))


# ==========================
# ZCR FUNCTION (ADDED)
# ==========================
def compute_zcr(frame):
    return np.sum(np.abs(np.diff(np.sign(frame)))) / (2 * len(frame))


# preprocessing
def preprocess_audio(file_path):
    signal, s_rate = librosa.load(file_path, sr=SAMPLE_RATE) #converts to mono, and resample to make Python features = ATmega features

    # Trim silence
    signal, _ = librosa.effects.trim(signal)

    # Normalize
    # if np.max(np.abs(signal)) > 0:
    #     signal = signal / np.max(np.abs(signal))

    return signal


# feature extraction
def extract_features(signal):
    rms_values = []
    zcrs = []
    envelopes = []

    # FRAME-BASED FEATURES
    for i in range(0, len(signal) - FRAME_SIZE + 1, HOP_SIZE):
        frame = signal[i:i + FRAME_SIZE]

        # RMS Energy
        rms = compute_rms(frame)
        rms_values.append(rms)

        # ZCR
        zcr = compute_zcr(frame)
        zcrs.append(zcr)

        # Amplitude Envelope (mean absolute value)
        envelope = np.mean(np.abs(frame))
        envelopes.append(envelope)

    # Avoid empty case
    if len(rms_values) == 0:
        return [0, 0, 0]

    # FRAME-BASED AVERAGING
    avg_rms = np.mean(rms_values)
    avg_zcr = np.mean(zcrs)
    avg_envelope = np.mean(envelopes)

    return [
        avg_rms,
        avg_zcr,
        avg_envelope
    ]


# build template
def build_templates():
    templates = []

    for word in WORDS:
        word_folder = os.path.join(DATASET_PATH, word)
        features_list = []

        for file in os.listdir(word_folder):
            if file.lower().endswith(".wav"):
                file_path = os.path.join(word_folder, file)
                print(f"Processing: {file_path}")

                signal = preprocess_audio(file_path)
                features = extract_features(signal)

                features_list.append(features)
                

        if len(features_list) == 0:
            print(f"[WARNING] No files found for word: {word}")
            templates.append([0, 0, 0])
        else:
            avg_features = np.mean(features_list, axis=0)
            templates.append(avg_features)

        print(f"Processed word: {word}")

    templates = np.array(templates)

    # features normalization
    # Normalize features across all words to reduce loudness effect
    mean = np.mean(templates, axis=0)
    std = np.std(templates, axis=0)

    # Avoid division by zero
    std[std == 0] = 1

    templates = (templates - mean) / std

    return templates, mean, std


# generate header file
def generate_header(templates, mean, std):
    with open(OUTPUT_HEADER, "w") as f:
        f.write("#ifndef TEMPLATES_H\n")
        f.write("#define TEMPLATES_H\n\n")

        f.write("#define NUM_WORDS 8\n")
        f.write("#define NUM_FEATURES 3\n\n")

        f.write("const float word_templates[NUM_WORDS][NUM_FEATURES] = {\n")

        for t in templates:
            f.write("    {")
            f.write(", ".join([f"{x:.6f}" for x in t]))
            f.write("},\n")

        f.write("};\n\n")

        # save normalization parameters
        f.write("const float feature_mean[NUM_FEATURES] = {\n")
        f.write("    " + ", ".join([f"{x:.6f}" for x in mean]) + "\n")
        f.write("};\n\n")

        f.write("const float feature_std[NUM_FEATURES] = {\n")
        f.write("    " + ", ".join([f"{x:.6f}" for x in std]) + "\n")
        f.write("};\n\n")

        f.write("#endif\n")

    print(f"[INFO] Header file generated: {OUTPUT_HEADER}")


if __name__ == "__main__":
    templates, mean, std = build_templates()
    generate_header(templates, mean, std)

    print("\n=== FINAL TEMPLATES ===")
    for word, t in zip(WORDS, templates):
        print(f"{word}: {t}")