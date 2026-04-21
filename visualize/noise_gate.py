import numpy as np
import wave
import sys

SAMPLE_RATE = 16000

def load_vad(vad_path):
    segments = []
    with open(vad_path, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) == 3:
                start = float(parts[0])
                end = float(parts[1])
                label = parts[2]
                segments.append((start, end, label))
    return segments


def noise_gate(input_wav, vad_path, output_wav):

    with wave.open(input_wav, 'rb') as wr:
        n_channels = wr.getnchannels()
        sampwidth = wr.getsampwidth()
        framerate = wr.getframerate()
        n_frames = wr.getnframes()
        audio_bytes = wr.readframes(n_frames)
    
    audio = np.frombuffer(audio_bytes, dtype=np.int16).astype(np.float32)
    audio = audio / 32768.0
    
    vad_segments = load_vad(vad_path)
    
    output = np.zeros_like(audio)
    
    prev_label = 'S'
    
    for i, (start, end, label) in enumerate(vad_segments):
        start_sample = int(start * SAMPLE_RATE)
        end_sample = int(end * SAMPLE_RATE)
        
        start_sample = max(0, start_sample)
        end_sample = min(len(audio), end_sample)
        
        segment = audio[start_sample:end_sample].copy()
        
        if label == 'V':
            
            if prev_label == 'S':
                pass
            
            output[start_sample:end_sample] = segment
        
        prev_label = label
    
    output_int16 = (output * 32767).astype(np.int16).tobytes()
    
    with wave.open(output_wav, 'wb') as ww:
        ww.setnchannels(n_channels)
        ww.setsampwidth(sampwidth)
        ww.setframerate(framerate)
        ww.writeframes(output_int16)
    
    print(f"Procesado: {input_wav} -> {output_wav}")
    print(f"  Duracion: {len(audio) / SAMPLE_RATE:.2f}s")


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(f"Uso: {sys.argv[0]} <input.wav> <input.vad> <output.wav>")
        sys.exit(1)
    
    input_wav = sys.argv[1]
    vad_path = sys.argv[2]
    output_wav = sys.argv[3]
    
    noise_gate(input_wav, vad_path, output_wav)
