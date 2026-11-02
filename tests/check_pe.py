from pathlib import Path
import struct,subprocess
p=Path(__file__).resolve().parents[1]/'ASuePotGame.exe';b=p.read_bytes();u16=lambda o:struct.unpack_from('<H',b,o)[0];u32=lambda o:struct.unpack_from('<I',b,o)[0]
assert b[:2]==b'MZ';offset=u32(0x3c);assert b[offset:offset+4]==b'PE\0\0';assert u16(offset+4)==0x8664;opt=offset+24;assert u16(opt)==0x20b;assert u16(opt+68)==2
count=u16(offset+6);sections=[]
for i in range(count):
 o=opt+u16(offset+20)+i*40;name=b[o:o+8].rstrip(b'\0').decode();size,rva,raw_size,raw=struct.unpack_from('<IIII',b,o+8);assert raw+raw_size<=len(b);sections.append((rva,size,raw,raw_size,name))
def pos(rva):
 for a,size,raw,rs,name in sections:
  if a<=rva<a+size:return raw+rva-a
 raise ValueError(hex(rva))
entry=u32(opt+16);assert any(a<=entry<a+s and n=='.text' for a,s,raw,rs,n in sections)
imports=u32(opt+120);o=pos(imports);dlls=[];apis=[]
while u32(o):
 table=u32(o);dll_rva=u32(o+12);iat=u32(o+16);p0=pos(dll_rva);end=b.index(b'\0',p0);dlls.append(b[p0:end].decode());t=pos(table);w=pos(iat)
 while struct.unpack_from('<Q',b,t)[0]:
  hint=struct.unpack_from('<Q',b,t)[0];assert hint<2**32;h=pos(hint)+2;end=b.index(b'\0',h);apis.append(b[h:end].decode());assert struct.unpack_from('<Q',b,w)[0]==hint;t+=8;w+=8
 o+=20
assert set(dlls)=={'KERNEL32.dll','USER32.dll','GDI32.dll','WINMM.dll','MSIMG32.dll'}
assert 'TransparentBlt' in apis and 'CreateWindowExW' in apis and 'MoveFileExW' in apis
print(f'PASS: PE32+ x64 Windows GUI, valid entry/sections/imports; {len(b)} bytes; {len(apis)} system API imports; no external runtime imports. This does not test execution.')
