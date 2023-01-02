# pip install -i https://pypi.tuna.tsinghua.edu.cn/simple chardet
import os
import re
from chardet import detect


def get_file_list(d):
    result = []
    for root, dirs, files in os.walk(d):
        dirs[:] = [d for d in dirs if d not in ['venv', 'cmake-build-debug']]
        for filename in files:
            # your filter
            if re.search(r'(\.c|\.cpp|\.h|\.txt)$', filename):
                result.append(os.path.join(root, filename))
    return result


# get file encoding type
def get_encoding_type(file):
    with open(file, 'rb') as f:
        raw_data = f.read()
    return detect(raw_data)['encoding']


if __name__ == "__main__":
    file_list = get_file_list('.')
    for src_file in file_list:
        print(src_file)
        trg_file = src_file + '.swp'
        from_codec = get_encoding_type(src_file)
        try:
            with open(src_file, 'r', encoding=from_codec) as f, open(trg_file, 'w', encoding='utf-8') as e:
                text = f.read()
                e.write(text)
            os.remove(src_file)
            os.rename(trg_file, src_file)
        except UnicodeDecodeError:
            print('Decode Error')
        except UnicodeEncodeError:
            print('Encode Error')
