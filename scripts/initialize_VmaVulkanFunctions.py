with open('../packages/VulkanMemoryAllocator/include/vk_mem_alloc.h') as f:
    lines = f.readlines()
    
    functions = []
    for i in range(954, 998):
        line = lines[i].strip()
        if line.startswith('PFN'):
            func = line.split(' ')[-1].strip(';')
            functions.append(func)
    
    print(',\n'.join(list(map(lambda f : '.{} = {}'.format(f, f.strip('KHR')), functions))))
