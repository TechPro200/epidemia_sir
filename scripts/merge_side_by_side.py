from PIL import Image
import os

# Rutas corregidas según tus carpetas actuales
SEQ_DIR = r'C:\Users\tutan\Downloads\epidemia_sir\output_seq'
PAR_DIR = r'C:\Users\tutan\Downloads\epidemia_sir\paralelo\output_par'  # si existe
OUT_DIR = r'C:\Users\tutan\Downloads\epidemia_sir\output_merge'

os.makedirs(OUT_DIR, exist_ok=True)

seq_files = sorted([f for f in os.listdir(SEQ_DIR) if f.endswith('.ppm')])

for f in seq_files:
    s = Image.open(os.path.join(SEQ_DIR, f)).convert('RGB')
    p = Image.open(os.path.join(PAR_DIR, f)).convert('RGB')
    w, h = s.size
    merged = Image.new('RGB', (w*2, h))
    merged.paste(s, (0,0))
    merged.paste(p, (w,0))
    merged.save(os.path.join(OUT_DIR, f))

print('Merged images into', OUT_DIR)
