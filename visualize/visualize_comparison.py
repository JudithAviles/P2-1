import numpy as np
import wave
import matplotlib.pyplot as plt
import seaborn as sns

sns.set_theme(style="whitegrid", rc={"figure.dpi": 150, "savefig.dpi": 300})


def load_wav(wav_path):
    with wave.open(wav_path, 'rb') as wr:
        framerate = wr.getframerate()
        n_frames = wr.getnframes()
        audio_bytes = wr.readframes(n_frames)
    
    audio = np.frombuffer(audio_bytes, dtype=np.int16).astype(np.float32)
    audio = audio / 32768.0
    
    duration = len(audio) / framerate
    time = np.linspace(0, duration, len(audio))
    
    return audio, time, framerate


def plot_comparison(original_wav, clean_wav, output_path="comparison.png"):

    audio_orig, time_orig, fs = load_wav(original_wav)
    audio_clean, time_clean, _ = load_wav(clean_wav)
    
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 6), sharex=True)
    
    ax1.plot(time_orig, audio_orig, color='#2E86AB', linewidth=0.5, alpha=0.8)
    ax1.set_ylabel('Amplitud', fontsize=11)
    ax1.set_title('Audio Original (con ruido de fondo)', fontsize=13, fontweight='bold')
    ax1.set_ylim(-1.1, 1.1)
    ax1.axhline(y=0, color='gray', linestyle='--', linewidth=0.3, alpha=0.5)
    
    ax2.plot(time_clean, audio_clean, color='#28A745', linewidth=0.5, alpha=0.8)
    ax2.set_ylabel('Amplitud', fontsize=11)
    ax2.set_xlabel('Tiempo (s)', fontsize=11)
    ax2.set_title('Audio Limpio (silencio a cero)', fontsize=13, fontweight='bold')
    ax2.set_ylim(-1.1, 1.1)
    ax2.axhline(y=0, color='gray', linestyle='--', linewidth=0.3, alpha=0.5)
    
    duration = time_clean[-1]
    tick_interval = 0.5 if duration < 10 else 1.0
    ticks = np.arange(0, duration + tick_interval, tick_interval)
    ax2.set_xticks(ticks)
    ax2.set_xlim(0, duration)
    
    plt.tight_layout()
    plt.savefig(output_path, bbox_inches='tight', facecolor='white')
    plt.close()
    
    print(f"Figura guardada: {output_path}")
    print(f"  Duracion: {duration:.2f}s | Fs: {fs}Hz")


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) < 3:
        print(f"Uso: {sys.argv[0]} <original.wav> <clean.wav> [output.png]")
        sys.exit(1)
    
    original_wav = sys.argv[1]
    clean_wav = sys.argv[2]
    output_path = sys.argv[3] if len(sys.argv) > 3 else "comparison.png"
    
    plot_comparison(original_wav, clean_wav, output_path)
