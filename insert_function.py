import argparse

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

    parser.add_argument('-n', '--name', type=str, required=True, help='API name, ie VirtualAlloc. IMPORTANT! Casing must be how the function is called!')
    parser.add_argument('-r', '--ret',  type=str, required=True, help='Return type, ie LPVOID')
    parser.add_argument('-a', '--args', type=str, required=True, help='argv for typedef, ie "LPVOID, SIZE_T, DWORD"')

    args = parser.parse_args()

    with open("apiresolver.h", "r") as f:
        header_data = f.readlines()
    
    with open("apiresolver.c", "r") as f:
        c_data = f.readlines()
    
    if f"Api{args.name}," in ''.join(header_data):
        print(f"Error: {args.name} is already found in header file.")
        exit()

    insert = f'typedef {args.ret.upper()} (WINAPI * {args.name}_T)( {args.args.upper()} );\n'
    header_data = insert_line(header_data, typedefs, insert)

    insert = f'#define {args.name} API_DEFINE({args.name})\n'
    header_data = insert_line(header_data, defines, insert)

    insert = f'    &{args.name},\n'
    c_data = insert_line(c_data, api_array, insert)

    insert = f'    Api{args.name},\n'
    header_data = insert_line(header_data, api_enum, insert)

    with open("apiresolver.h", 'w') as file:
        file.writelines(header_data)
    
    with open("apiresolver.c", 'w') as file:
        file.writelines(c_data)