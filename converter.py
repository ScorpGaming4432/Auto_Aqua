# lcd_encode.py
#
# Converts readable Unicode strings into LCD-byte-strings
# for Arduino/C++ source code.
#
# Example:
#   Input:  "Num=Язык"
#   Output: "Num=\xad\x83\x8e\x6b"

import json

# -------------------------
# 1. DEFINE YOUR LCD MAPPING
# -------------------------

lcd_map = {
    # --- Custom characters ---
    "␀": 0x00,  # use ␀ in your text to mean custom char 0
    "␁": 0x01,
    "␂": 0x02,
    "␃": 0x03,
    "␄": 0x04,
    "␅": 0x05,
    "␆": 0x06,
    "␇": 0x07,
    
    " ": 0o040,"!": 0o041,"\"": 0o042,"#": 0o043,"$": 0o044,"%": 0o045,"&": 0o046,"'": 0o047,"(": 0o050,")": 0o051,"*": 0o052,"+": 0o053,",": 0o054,"-": 0o055,".": 0o056,"/": 0o057,"0": 0o060,"1": 0o061,"2": 0o062,"3": 0o063,"4": 0o064,"5": 0o065,"6": 0o066,"7": 0o067,"8": 0o070,"9": 0o071,":": 0o072,";": 0o073,"<": 0o074,"=": 0o075,">": 0o076,"?": 0o077,"@": 0o100,"A": 0o101,"B": 0o102,"C": 0o103,"D": 0o104,"E": 0o105,"F": 0o106,"G": 0o107,"H": 0o110,"I": 0o111,"J": 0o112,"K": 0o113,"L": 0o114,"M": 0o115,"N": 0o116,"O": 0o117,"P": 0o120,"Q": 0o121,"R": 0o122,"S": 0o123,"T": 0o124,"U": 0o125,"V": 0o126,"W": 0o127,"X": 0o130,"Y": 0o131,"Z": 0o132,"[": 0o133,"\\": 0o134,"]": 0o135,"^": 0o136,"_": 0o137,"`": 0o140,"a": 0o141,"b": 0o142,"c": 0o143,"d": 0o144,"e": 0o145,"f": 0o146,"g": 0o147,"h": 0o150,"i": 0o151,"j": 0o152,"k": 0o153,"l": 0o154,"m": 0o155,"n": 0o156,"o": 0o157,"p": 0o160,"q": 0o161,"r": 0o162,"s": 0o163,"t": 0o164,"u": 0o165,"v": 0o166,"w": 0o167,"x": 0o170,"y": 0o171,"z": 0o172,"{": 0o173,"|": 0o174,"}": 0o175,"~": 0o176,
    # --- Cyrillic mappings (example) ---
    # Fill these in with YOUR LCD's actual byte table:
    "Δ": 0x7F,
    "Б": 0x80,
    "Д": 0x81,
    "Ж": 0x82,
    "З": 0x83,
    "И": 0x84,
    "Й": 0x85,
    "Л": 0x86,
    "П": 0x87,
    "У": 0x88,
    "Ц": 0x89,
    "Ч": 0x8a,
    "Ш": 0x8b,
    "Щ": 0x8c,
    "Ъ": 0x8d,
    "Ы": 0x8e,
    "Э": 0x8f,
    "α": 0x90,
    "♪": 0x91,
    "Γ": 0x92,
    "π": 0x93,
    "Σ": 0x94,
    "σ": 0x95,
    "♬": 0x96,
    "τ": 0x97,
    "🔔": 0x98,
    "θ": 0x99,
    "Ω": 0x9a,
    "δ": 0x9b,
    "∞": 0x9c,
    "♥": 0x9d,
    "ξ": 0x9e,
    "∩": 0x9f,
    "⏸": 0xa0,
    "¡": 0xa1,
    "¢": 0xa2,
    "£": 0xa3,
    "⦻": 0xa4,
    "¥": 0xa5,
    "¦": 0xa6,
    "§": 0xa7,
    "ƒ": 0xa8,
    "©": 0xa9,
    "ª": 0xaa,
    "«": 0xab,
    "Ю": 0xac,
    "Я": 0xad,
    "®": 0xae,
    "´": 0xaf,
    "°": 0xb0,
    "±": 0xb1,
    "²": 0xb2,
    "³": 0xb3,
    "Pt": 0xb4,
    "µ": 0xb5,
    "¶": 0xb6,
    "·": 0xb7,
    "ω": 0xb8,
    "¹": 0xb9,
    "º": 0xba,
    "»": 0xbb,
    "¼": 0xbc,
    "½": 0xbd,
    "¾": 0xbe,
    "¿": 0xbf,
    "À": 0xc0,
    "Á": 0xc1,
    "Â": 0xc2,
    "Ã": 0xc3,
    "Ä": 0xc4,
    "Å": 0xc5,
    "Æ": 0xc6,
    "Ç": 0xc7,
    "È": 0xc8,
    "É": 0xc9,
    "Ê": 0xca,
    "Ë": 0xcb,
    "Ì": 0xcc,
    "Í": 0xcd,
    "Î": 0xce,
    "Ï": 0xcf,
    "Ð": 0xd0,
    "Ñ": 0xd1,
    "Ò": 0xd2,
    "Ó": 0xd3,
    "Ô": 0xd4,
    "Õ": 0xd5,
    "Ö": 0xd6,
    "×": 0xd7,
    "ɸ": 0xd8,
    "Ù": 0xd9,
    "Ú": 0xda,
    "Û": 0xdb,
    "Ü": 0xdc,
    "Ý": 0xdd,
    "Þ": 0xde,
    "ß": 0xdf,
    "à": 0xe0,
    "á": 0xe1,
    "â": 0xe2,
    "ã": 0xe3,
    "ä": 0xe4,
    "å": 0xe5,
    "æ": 0xe6,
    "ç": 0xe7,
    "è": 0xe8,
    "é": 0xe9,
    "ê": 0xea,
    "ë": 0xeb,
    "ì": 0xec,
    "í": 0xed,
    "î": 0xee,
    "ï": 0xef,
    "ð": 0xf0,
    "ñ": 0xf1,
    "ò": 0xf2,
    "ó": 0xf3,
    "ô": 0xf4,
    "õ": 0xf5,
    "ö": 0xf6,
    "÷": 0xf7,
    "ø": 0xf8,
    "ù": 0xf9,
    "ú": 0xfa,
    "û": 0xfb,
    "ü": 0xfc,
    "ý": 0xfd,
    "þ": 0xfe,
    "ÿ": 0xff
    # add as needed...
}

# -------------------------
# 2. TEXT TO CONVERT
# -------------------------

strings = [
    "tina  ", "Num=Jazyk  ", "Objem nádrže  " 
]

# -------------------------
# 3. ENCODER
# -------------------------

def encode_lcd(s: str):
    out = ""
    for ch in s:
        if ch.lower() in lcd_map or ch.upper() in lcd_map:
            b = lcd_map[ch]
            # always output hex escapes to avoid NUL issues
            print(f"\\{oct(b)[2:].rjust(3,"0")}",end='')
        elif 32 <= ord(ch) <= 126:
            # printable ASCII, keep as-is
            print(ch,end="")
        else:
            raise ValueError(f"Character not mapped: {repr(ch)}")
    return out

for s in strings:
  print("\"",end="")
  encode_lcd(s)
  print("\"",end=", ")