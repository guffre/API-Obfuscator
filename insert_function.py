import argparse
import re

typedefs = "// typedefs 9711\n"
defines = "// defines e56b\n"
api_array = "PVOID ApiResolverAPIs[] = {\n"
api_enum = "enum ApiOffsetOrder {\n"

def insert_line(data, search, insert):
    line   = data.index(search)
    data.insert(line+1, insert)
    return data

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Inserts a function in apiresolver.h')

    parser.add_argument('-n', '--name',   type=str, help='API name, ie VirtualAlloc. IMPORTANT! Casing must be how the function is called!')
    parser.add_argument('-r', '--ret', type=str, help='Return type, ie LPVOID')
    parser.add_argument('-a', '--args',   type=str, help='argv for typedef, ie "LPVOID, SIZE_T, DWORD"')

    args = parser.parse_args()

    with open("apiresolver.h", "r") as f:
        data = f.readlines()
    
    sanity_check = len(''.join(data))

    insert = f'typedef {args.ret.upper()} (WINAPI * {args.name}_T)( {args.args.upper()} );\n'
    data = insert_line(data, typedefs, insert)

    insert = f'#define {args.name} API_DEFINE({args.name})\n'
    data = insert_line(data, defines, insert)

    insert = f'    &{args.name},\n'
    data = insert_line(data, api_array, insert)

    insert = f'    Api{args.name},\n'
    data = insert_line(data, api_enum, insert)

    if len(''.join(data)) < sanity_check:
        print("Something went wrong!")
    else:
        with open("apiresolver.h", 'w') as file:
            file.writelines(data)