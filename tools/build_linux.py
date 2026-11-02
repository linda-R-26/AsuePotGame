"""在现有 GNU x64 工具链构建无 CRT 的 Win64 PE；不依赖下载。"""
from pathlib import Path
import re,subprocess,struct
root=Path(__file__).resolve().parents[1];build=root/'tools'/'build';build.mkdir(exist_ok=True)
header=(root/'project/win_api.h').read_text()
names=re.findall(r'API\([^,]+,(\w+),',header);names=[n for n in names if n!='name']
names+=['PlaySoundW','mciSendStringW','waveOutSetVolume','TransparentBlt','GetDC','ReleaseDC','UpdateWindow','GetMessageW','SetTimer','KillTimer','GetTickCount64','BitBlt']
user=set('RegisterClassW CreateWindowExW ShowWindow PeekMessageW TranslateMessage DispatchMessageW DefWindowProcW PostQuitMessage InvalidateRect BeginPaint EndPaint GetClientRect SetWindowPos SetWindowLongPtrW LoadCursorW LoadImageW MessageBoxW GetSystemMetrics GetDC ReleaseDC UpdateWindow GetMessageW SetTimer KillTimer'.split())
gdi=set('CreateCompatibleDC CreateCompatibleBitmap SelectObject DeleteObject DeleteDC CreateSolidBrush CreatePen Rectangle RoundRect Ellipse MoveToEx LineTo TextOutW SetTextColor SetBkMode CreateFontW StretchBlt StretchDIBits BitBlt'.split())
groups={'KERNEL32.dll':[],'USER32.dll':[],'GDI32.dll':[],'WINMM.dll':[],'MSIMG32.dll':[]}
for n in names:groups['MSIMG32.dll' if n=='TransparentBlt' else 'USER32.dll' if n in user else 'GDI32.dll' if n in gdi else 'WINMM.dll' if n in ['PlaySoundW','mciSendStringW','waveOutSetVolume'] else 'KERNEL32.dll'].append(n)
a=['.section .idata$2,"aw"','.global import_start','import_start:']
for i,(dll,ns) in enumerate(groups.items()):a +=[f'.long ilt{i}-0x140000000','.long 0,0',f'.long dll{i}-0x140000000',f'.long iat{i}-0x140000000']
a+=['.section .idata$3,"aw"','.long 0,0,0,0,0']
for i,(dll,ns) in enumerate(groups.items()):
 a+=['.section .idata$4,"aw"',f'ilt{i}:']+[f'.quad hint_{n}-0x140000000' for n in ns]+['.quad 0','.section .idata$5,"aw"',f'iat{i}:']
 for n in ns:a +=[f'.global __imp_{n}',f'__imp_{n}: .quad hint_{n}-0x140000000']
 a+=['.quad 0','.section .idata$6,"aw"',f'dll{i}: .asciz "{dll}"']
 for n in ns:a +=['.balign 2',f'hint_{n}: .short 0',f'.asciz "{n}"']
a+=['.section .note.GNU-stack,"",@progbits'];(build/'imports.s').write_text('\n'.join(a)+'\n')
(build/'link.ld').write_text('SECTIONS { . = 0x140001000; .text : { *(.text*) } . = ALIGN(4096); .rdata : { *(.rodata*) } . = ALIGN(4096); .idata : { *(SORT_BY_NAME(.idata$*)) } . = ALIGN(4096); .data : { *(.data*) *(.bss*) *(COMMON) } /DISCARD/ : { *(.eh_frame*) *(.comment*) *(.note*) *(.init_array*) } }')
def run(*a):subprocess.run(a,check=True)
run('g++','-c','-O2','-mabi=ms','-maccumulate-outgoing-args','-mcmodel=large','-fshort-wchar','-ffreestanding','-fno-pic','-fno-builtin','-fno-stack-protector','-fno-asynchronous-unwind-tables','-fno-exceptions','-fno-rtti',str(root/'project/main.cpp'),'-o',str(build/'main.o'))
run('as',str(build/'imports.s'),'-o',str(build/'imports.o'))
run('ld','-T',str(build/'link.ld'),str(build/'main.o'),str(build/'imports.o'),'-o',str(build/'game.elf'))
symbols={line.split()[-1]:int(line.split()[0],16) for line in subprocess.check_output(['nm',str(build/'game.elf')],text=True).splitlines() if len(line.split())==3}
sections=[];base=0x140000000
for line in subprocess.check_output(['objdump','-h',str(build/'game.elf')],text=True).splitlines():
 m=re.match(r'\s*\d+\s+(\.\w+)\s+([0-9a-f]+)\s+([0-9a-f]+)',line)
 if m:
  name,size,va=m[1],int(m[2],16),int(m[3],16)
  if not size:continue
  target=build/(name+'.bin');run('objcopy','--dump-section',f'{name}={target}',str(build/'game.elf'));sections.append((name,size,va-base,target.read_bytes()))
def align(v,a):return (v+a-1)//a*a
header_size=align(0x80+4+20+240+40*len(sections),512);size_image=align(max(va+size for _,size,va,_ in sections),4096)
dos=bytearray(0x80);dos[:2]=b'MZ';struct.pack_into('<I',dos,0x3c,0x80)
coff=struct.pack('<HHIIIHH',0x8664,len(sections),0,0,0,240,0x0023) # reloc stripped / executable / large address
opt=bytearray(240);struct.pack_into('<HBB',opt,0,0x20b,1,0)
struct.pack_into('<III',opt,4,align(sections[0][1],512),sum(align(s[1],512) for s in sections[1:]),0)
struct.pack_into('<IIQII',opt,16,symbols['entry']-base,sections[0][2],base,4096,512)
struct.pack_into('<HHHHHH',opt,40,6,0,1,0,6,0)
struct.pack_into('<IIIHH',opt,56,size_image,header_size,0,2,0x0100) # Windows GUI, NX, fixed base
struct.pack_into('<QQQQII',opt,72,4*1024*1024,1024*1024,1024*1024,65536,0,16)
struct.pack_into('<II',opt,112+8,symbols['import_start']-base,20*(len(groups)+1))
raw=header_size;table=b'';chunks=[]
for name,size,va,data in sections:
 data=data.ljust(align(len(data),512),b'\0');flags=0x60000020 if name=='.text' else 0xc0000040 if name in ['.data','.idata'] else 0x40000040
 table+=struct.pack('<8sIIIIIIHHI',name.encode(),size,va,len(data),raw,0,0,0,0,flags);chunks.append(data);raw+=len(data)
pe=(bytes(dos)+b'PE\0\0'+coff+bytes(opt)+table).ljust(header_size,b'\0')+b''.join(chunks)
(root/'ASuePotGame.exe').write_bytes(pe);print('Built',len(pe),'bytes')
