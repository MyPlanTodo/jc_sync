/*
 * Copyright Inria:
 * Jean-François Hren
 * 
 * jfhren[at]gmail[dot]com
 * michael[dot]hauspie[at]lifl[dot]com
 * 
 * This software is a computer program whose purpose is to read, analyze,
 * modify, generate and write Java Card 2 CAP file.
 * 
 * This software is governed by the CeCILL-B license under French
 * law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the
 * CeCILL-B
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its
 * terms.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <analyzed_cap_file.h>
#include <exp_file.h>

static export_file* get_export_file(export_file** export_files, int nb_export_files, const char* fully_qualified_class_name) {

    int i = 0;
    char* crt_position = NULL;

    char name[0XFFFFu];
    strcpy(name, fully_qualified_class_name);
    crt_position = name;

    while((crt_position = strchr(crt_position, '.')) != NULL) {
        *crt_position = '/';
        ++crt_position;
    }

    crt_position = strrchr(name, '/');
    if(crt_position == NULL)
        return NULL;
    *crt_position = '\0';

    for(; i < nb_export_files; ++i)
        if((export_files[i]->constant_pool[export_files[i]->constant_pool[export_files[i]->this_package].CONSTANT_Package.name_index].CONSTANT_Utf8.length == strlen(name)) && (strncmp((char*)export_files[i]->constant_pool[export_files[i]->constant_pool[export_files[i]->this_package].CONSTANT_Package.name_index].CONSTANT_Utf8.bytes, name, strlen(name)) == 0))
            return export_files[i];

    return NULL;

}


int get_class_token_from_export_file(export_file* ef, const char* fully_qualified_class_name, u1* token) {

    u2 u2Index = 0;
    char* crt_position = NULL;

    char name[0XFFFFu];
    int name_length = 0;
    strcpy(name, fully_qualified_class_name);
    crt_position = name;

    while((crt_position = strchr(crt_position, '.')) != NULL) {
        *crt_position = '/';
        ++crt_position;
    }

    name_length = strlen(name);

    for(; u2Index < ef->constant_pool_count; ++u2Index)
        if(ef->constant_pool[u2Index].tag == EF_CONSTANT_CLASSREF)
            if((name_length == ef->constant_pool[ef->constant_pool[u2Index].CONSTANT_Classref.name_index].CONSTANT_Utf8.length) && (strncmp(name, (char*)ef->constant_pool[ef->constant_pool[u2Index].CONSTANT_Classref.name_index].CONSTANT_Utf8.bytes, ef->constant_pool[ef->constant_pool[u2Index].CONSTANT_Classref.name_index].CONSTANT_Utf8.length) == 0)) {
                u1 u1Index = 0;

                for(; u1Index < ef->export_class_count; ++u1Index)
                    if(ef->classes[u1Index].name_index == u2Index) {
                        *token = ef->classes[u1Index].token;
                        return 0;
                    }

                break;
            }

    fprintf(stderr, "Could not find the class token by its fully qualified class name\n");
    return -1;

}


int get_method_token_from_export_file(export_file* ef, u1 class_token, const char* method_name, const char* method_signature, u1* token) {

    u2 u2Index1 = 0;
    u2 method_name_length = strlen(method_name);
    u2 method_signature_length = strlen(method_signature);

    for(; u2Index1 < ef->export_class_count; ++u2Index1)
        if(ef->classes[u2Index1].token == class_token) {
            u2 u2Index2 = 0;

            for(; u2Index2 < ef->classes[u2Index1].export_methods_count; ++u2Index2)
                if((method_name_length == ef->constant_pool[ef->classes[u2Index1].methods[u2Index2].name_index].CONSTANT_Utf8.length) && (strncmp(method_name, (char*)ef->constant_pool[ef->classes[u2Index1].methods[u2Index2].name_index].CONSTANT_Utf8.bytes, method_name_length) == 0) && (method_signature_length == ef->constant_pool[ef->classes[u2Index1].methods[u2Index2].descriptor_index].CONSTANT_Utf8.length) && (strncmp(method_signature, (char*)ef->constant_pool[ef->classes[u2Index1].methods[u2Index2].descriptor_index].CONSTANT_Utf8.bytes, method_signature_length) == 0)) {
                    *token = ef->classes[u2Index1].methods[u2Index2].token;
                    return 0;
                }
            break;
        }

    fprintf(stderr, "Could not find the method token by its class token, name and signature\n");
    return -1;

}


imported_package_info* add_imported_package_to_analyzed_cap_file(analyzed_cap_file* acf, export_file* ef) {

    u1 u1Index1 = 0;
    imported_package_info** tmp = NULL;

    for(; u1Index1 < acf->imported_packages_count; ++u1Index1)
        if(acf->imported_packages[u1Index1]->aid_length == ef->constant_pool[ef->this_package].CONSTANT_Package.aid_length) {
            u1 u1Index2 = 0;

            for(; u1Index2 < acf->imported_packages[u1Index1]->aid_length; ++u1Index2)
                if(acf->imported_packages[u1Index1]->aid[u1Index2] != ef->constant_pool[ef->this_package].CONSTANT_Package.aid[u1Index2])
                    break;

            if(u1Index2 == acf->imported_packages[u1Index1]->aid_length)
                return acf->imported_packages[u1Index1];
        }

    tmp = (imported_package_info**)realloc(acf->imported_packages, sizeof(imported_package_info*) * (acf->imported_packages_count + 1));
    if(tmp == NULL) {
        perror("add_imported_package_to_analyzed_cap_file");
        return NULL;
    }
    acf->imported_packages = tmp;

    acf->imported_packages[acf->imported_packages_count] = (imported_package_info*)malloc(sizeof(imported_package_info));
    if(acf->imported_packages[acf->imported_packages_count] == NULL) {
        perror("add_imported_package_to_analyzed_cap_file");
        return NULL;
    }

    acf->imported_packages[acf->imported_packages_count]->my_index = acf->imported_packages_count;
    acf->imported_packages[acf->imported_packages_count]->count = 0;
    acf->imported_packages[acf->imported_packages_count]->minor_version = ef->constant_pool[ef->this_package].CONSTANT_Package.minor_version;
    acf->imported_packages[acf->imported_packages_count]->major_version = ef->constant_pool[ef->this_package].CONSTANT_Package.major_version;
    acf->imported_packages[acf->imported_packages_count]->aid_length = ef->constant_pool[ef->this_package].CONSTANT_Package.aid_length;
    acf->imported_packages[acf->imported_packages_count]->aid = (u1*)malloc(acf->imported_packages[acf->imported_packages_count]->aid_length);
    if(acf->imported_packages[acf->imported_packages_count]->aid == NULL) {
        perror("add_imported_package_to_analyzed_cap_file");
        return NULL;
    }
    memcpy(acf->imported_packages[acf->imported_packages_count]->aid, ef->constant_pool[ef->this_package].CONSTANT_Package.aid, acf->imported_packages[acf->imported_packages_count]->aid_length);
    acf->imported_packages[acf->imported_packages_count]->ef = ef;

    return acf->imported_packages[acf->imported_packages_count++];

}


constant_pool_entry_info* get_external_classref_from_constant_pool(analyzed_cap_file* acf, imported_package_info* imported_package, const char* fully_qualified_class_name, export_file** export_files, int nb_export_files) {

    u2 u2Index1 = 0;
    u1 class_token = 0;
    constant_pool_entry_info** tmp = NULL;
    export_file* ef = NULL;

    if(imported_package == NULL) {
        ef = get_export_file(export_files, nb_export_files, fully_qualified_class_name);
        if(ef == NULL)
            return NULL;

        if((imported_package = add_imported_package_to_analyzed_cap_file(acf, ef)) == NULL)
            return NULL;
    }

    if(get_class_token_from_export_file(imported_package->ef, fully_qualified_class_name, &class_token) == -1)
        return NULL;

    for(u2Index1 = 0; u2Index1 < acf->constant_pool_count; ++u2Index1)
        if((acf->constant_pool[u2Index1]->flags & CONSTANT_POOL_CLASSREF) && (acf->constant_pool[u2Index1]->flags & CONSTANT_POOL_IS_EXTERNAL) && (acf->constant_pool[u2Index1]->external_package->my_index == imported_package->my_index) && (acf->constant_pool[u2Index1]->external_class_token == class_token))
            return acf->constant_pool[u2Index1];

    tmp = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp == NULL) {
        perror("get_external_classref_from_constant_pool");
        return NULL;
    }
    acf->constant_pool = tmp;

    acf->constant_pool[acf->constant_pool_count] = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(acf->constant_pool[acf->constant_pool_count] == NULL) {
        perror("get_external_classref_from_constant_pool");
        return NULL;
    }

    acf->constant_pool[acf->constant_pool_count]->flags = CONSTANT_POOL_CLASSREF|CONSTANT_POOL_IS_EXTERNAL;
    acf->constant_pool[acf->constant_pool_count]->my_index = acf->constant_pool_count;
    acf->constant_pool[acf->constant_pool_count]->count = 0;
    acf->constant_pool[acf->constant_pool_count]->external_package = imported_package;
    acf->constant_pool[acf->constant_pool_count]->external_class_token = class_token;

    return acf->constant_pool[acf->constant_pool_count++];
    
}


u2 is_type_descriptor_not_unique(analyzed_cap_file* acf, type_descriptor_info* type) {

    u2 u2Index = 0;

    for(; u2Index < acf->signature_pool_count; ++u2Index)
        if(type->types_count == acf->signature_pool[u2Index]->types_count) {
            u1 u1Index = 0;

            for(; u1Index < type->types_count; ++u1Index) {
                if(type->types[u1Index].type != acf->signature_pool[u2Index]->types[u1Index].type)
                    break;
                if(type->types[u1Index].type & TYPE_DESCRIPTOR_REF) {
                    if(type->types[u1Index].ref->flags != acf->signature_pool[u2Index]->types[u1Index].ref->flags)
                        break;
                    if(type->types[u1Index].ref->flags & CONSTANT_POOL_IS_EXTERNAL) {
                        if((type->types[u1Index].ref->external_package->my_index != acf->signature_pool[u2Index]->types[u1Index].ref->external_package->my_index)
                           || (type->types[u1Index].ref->external_class_token != acf->signature_pool[u2Index]->types[u1Index].ref->external_class_token))
                            break;
                    } else {
                        if((type->types[u1Index].ref->internal_class != acf->signature_pool[u2Index]->types[u1Index].ref->internal_class) || (type->types[u1Index].ref->internal_interface != acf->signature_pool[u2Index]->types[u1Index].ref->internal_interface))
                            break;
                    }
                }
            }

            if(u1Index == type->types_count)
                return u2Index;

        }

    return u2Index;

}


method_info* add_method(analyzed_cap_file* acf, class_info* class, type_descriptor_info* signature, u1 flags) {

    method_info** tmp1 = NULL;
    constant_pool_entry_info** tmp2 = NULL;
    method_info* method = (method_info*)malloc(sizeof(method_info));
    if(method == NULL) {
        perror("add_method");
        return NULL;
    }

    memset(method, 0, sizeof(method_info));

    method->this_method = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(method->this_method == NULL) {
        perror("add_method");
        return NULL;
    }

    method->flags = flags;

    if(flags & (METHOD_STATIC|METHOD_INIT))
        method->this_method->flags = CONSTANT_POOL_STATICMETHODREF;
    else
        method->this_method->flags = CONSTANT_POOL_VIRTUALMETHODREF;

    method->this_method->my_index = 0;
    method->this_method->count = 0;
    method->this_method->type = signature;
    method->this_method->internal_class = class;
    method->this_method->internal_method = method;

    method->signature = signature;

    tmp1 = (method_info**)realloc(class->methods, sizeof(method_info*) * (class->methods_count + 1));
    if(tmp1 == NULL) {
        perror("add_method");
        return NULL;
    }
    class->methods = tmp1;

    class->methods[class->methods_count] = method;
    ++class->methods_count;

    tmp2 = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp2 == NULL) {
        perror("add_method");
        return NULL;
    }
    acf->constant_pool = tmp2;

    acf->constant_pool[acf->constant_pool_count] = method->this_method;
    ++acf->constant_pool_count;

    return method;

}


method_info* add_overriding_method(analyzed_cap_file* acf, class_info* class, type_descriptor_info* signature, u1 overrided_method_token) {

    method_info* method = add_method(acf, class, signature, METHOD_PUBLIC);

    if(method == NULL)
        return NULL;

    method->flags |= METHOD_PUBLIC;
    method->is_overriding = 1;
    method->token = overrided_method_token;

    return method;

}


constant_pool_entry_info* add_supercall_to_constant_pool(analyzed_cap_file* acf, class_info* class, u1 overrided_method_token, method_info* called_method, type_descriptor_info* method_signature) {

    constant_pool_entry_info** tmp = NULL;
    constant_pool_entry_info* entry = NULL;

    entry = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(entry == NULL) {
        perror("add_supercall_to_constant_pool");
        return NULL;
    }
    memset(entry, 0, sizeof(constant_pool_entry_info));
    entry->flags = CONSTANT_POOL_SUPERMETHODREF;
    entry->internal_class = class;
    entry->type = method_signature;
    entry->method_token = overrided_method_token;
    entry->internal_method = called_method;

    tmp = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp == NULL) {
        perror("add_super_call");
        return NULL;
    }
    acf->constant_pool = tmp;

    acf->constant_pool[acf->constant_pool_count] = entry;
    ++acf->constant_pool_count;

    return entry;

}


int is_internal_class_implementing_interface(constant_pool_entry_info* class_ref, constant_pool_entry_info* interface_ref, implemented_method_info** implemented_methods) {

    u1 u1Index = 0;

    if(class_ref->flags & CONSTANT_POOL_IS_EXTERNAL)
        return 0;

    if(class_ref->internal_class != NULL)
        for(; u1Index < class_ref->internal_class->interfaces_count; ++u1Index)
            if(interface_ref == class_ref->internal_class->interfaces[u1Index].ref) {
                if(implemented_methods != NULL)
                    *implemented_methods = class_ref->internal_class->interfaces[u1Index].index;
                return 1;
            }

    return 0;

}


int is_external_class_implementing_interface(constant_pool_entry_info* class_ref, const char* fully_qualified_interface_name) {

    u1 u1Index1 = 0;
    u1 class_token = 0;
    export_file* ef = NULL;
    u2 fully_qualified_interface_name_length = strlen(fully_qualified_interface_name);

    if(!(class_ref->flags & CONSTANT_POOL_IS_EXTERNAL))
        return 0;

    ef = class_ref->external_package->ef;
    class_token = class_ref->external_class_token;

    for(; u1Index1 < ef->export_class_count; ++u1Index1)
        if(ef->classes[u1Index1].token == class_token) {
            u1 u1Index2 = 0;
            for(; u1Index2 < ef->classes[u1Index1].export_interfaces_count; ++u1Index2)
                if((ef->constant_pool[ef->constant_pool[ef->classes[u1Index1].interfaces[u1Index2]].CONSTANT_Classref.name_index].CONSTANT_Utf8.length == fully_qualified_interface_name_length) && (strncmp((char*)ef->constant_pool[ef->constant_pool[ef->classes[u1Index1].interfaces[u1Index2]].CONSTANT_Classref.name_index].CONSTANT_Utf8.bytes, fully_qualified_interface_name, fully_qualified_interface_name_length) == 0))
                    return 1;
            return 0;
        }

    return 0;

}
