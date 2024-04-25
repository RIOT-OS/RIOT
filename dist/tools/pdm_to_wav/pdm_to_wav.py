import argparse
import json
import wave
import struct

try:
    import matplotlib.pyplot as plt
except ImportError:
    plt = None


def parse_samples(fname: str):
    with open(fname, "r") as f:
        data = f.read().encode("ascii")
    for line in data.splitlines():
        try:
            data = json.loads(line)
            if "samples" in data and "expected_sample_rate_in_Hz":
                return (data["samples"], data["expected_sample_rate_in_Hz"])
        except json.JSONDecodeError:
            continue
    raise ValueError(f"Could not find samples and sample rate in {fname}")


def samples_to_wav(samples, sample_rate, output_file):
    # Normalize samples to between -1 and 1
    max_sample = max(abs(sample) for sample in samples)
    samples = [sample / max_sample for sample in samples]

    # Convert to 16 bit integers
    audio = [int(sample * (2**15 - 1)) for sample in samples]

    # Save the data as a wave format
    with wave.open(f"{output_file}.wav", "w") as f:
        # Mono Channel
        f.setnchannels(1)
        # 2 bytes per sample
        f.setsampwidth(2)
        f.setframerate(sample_rate)
        # Convert list of ints to bytes
        f.writeframes(struct.pack("<" + "h" * len(audio), *audio))


def samples_to_fig(samples, output_file):
    # Plot the sliced data
    plt.plot(samples)

    plt.xlabel("Number of Sample")
    plt.ylabel("Signal Value", rotation=90)

    # Adjusting the limits of the axes for more detail
    plt.xlim(0, len(samples))
    plt.ylim(min(samples), max(samples))
    plt.grid(True)
    plt.show()
    plt.savefig(f"{output_file}.png")
    print(f"Graph saved as {output_file}.png")


def main():
    parser = argparse.ArgumentParser(
        description="Generates a WAV file from output of tests/periph/pdm firmware."
    )
    parser.add_argument(
        "file", type=str, help="Path to the log file containing the PDM data."
    )
    parser.add_argument(
        "--output-file",
        type=str,
        default="output",
        help="Path to the output file(s) to be created, excluding extention.",
    )
    args = parser.parse_args()
    samples, sample_rate = parse_samples(args.file)
    samples_to_wav(samples, sample_rate, args.output_file)
    if plt:
        samples_to_fig(samples, args.output_file)


if __name__ == "__main__":
    main()
