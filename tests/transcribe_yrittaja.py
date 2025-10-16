import whisper

# Load the model (tiny, base, small, medium, large)
model = whisper.load_model("small")

# Transcribe the audio file
result = model.transcribe("yrittaja.m4a")

# Print the transcript
print(result["text"])

