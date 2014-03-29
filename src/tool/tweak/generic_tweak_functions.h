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

/**
 * \file generic_tweak_functions.h
 */

#ifndef GENERIC_TWEAK_FUNCTIONS_H
#define GENERIC_TWEAK_FUNCTIONS_H

#include <analyzed_cap_file.h>
#include <exp_file.h>

/**
 * Get the class token from an export file given its fully qualified name.
 */
int get_class_token_from_export_file(export_file* ef, const char* fully_qualified_class_name, u1* token);

/**
 * Get the method token from an export file given its name and signature.
 */
int get_method_token_from_export_file(export_file* ef, u1 class_token, const char* method_name, const char* method_signature, u1* token);

imported_package_info* add_imported_package_to_analyzed_cap_file(analyzed_cap_file* acf, export_file* ef);

constant_pool_entry_info* get_external_classref_from_constant_pool(analyzed_cap_file* acf, imported_package_info* imported_package, const char* fully_qualified_class_name, export_file** export_files, int nb_export_files);

u2 is_type_descriptor_not_unique(analyzed_cap_file* acf, type_descriptor_info* type);

method_info* add_method(analyzed_cap_file* acf, class_info* class, type_descriptor_info* signature, u1 flags);

method_info* add_overriding_method(analyzed_cap_file* acf, class_info* class, type_descriptor_info* signature, u1 overrided_method_token);

constant_pool_entry_info* add_supercall_to_constant_pool(analyzed_cap_file* acf, class_info* class, u1 overrided_method_token, method_info* called_method, type_descriptor_info* method_signature);

int is_internal_class_implementing_interface(constant_pool_entry_info* class_ref, constant_pool_entry_info* interface_ref, implemented_method_info** implemented_methods);

int is_external_class_implementing_interface(constant_pool_entry_info* class_ref, const char* fully_qualified_interface_name);

#endif
