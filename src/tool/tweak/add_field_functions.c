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
 * \file add_field_functions.c
 * \brief Provide a few functions for adding a static or instance field to a
 * class.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <analyzed_cap_file.h>

#include "tweak_info.h"
#include "generic_tweak_functions.h"

static type_descriptor_info* boolean_type = NULL;
static type_descriptor_info* short_type = NULL;

constant_pool_entry_info* add_boolean_field(analyzed_cap_file* acf, class_info* class, u1 flags);
constant_pool_entry_info* add_short_field(analyzed_cap_file* acf, class_info* class, u1 flags);
constant_pool_entry_info* add_ref_field(analyzed_cap_file* acf, class_info* class, constant_pool_entry_info* ref, type_descriptor_info** type, u1 flags);


/**
 * \brief Add a boolean field to a class including its entries in the signature
 * and constant pools.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the field is added.
 * \param flags The flags of the new field.
 *
 * \return Return the newly created analyzed constant pool entry to the added
 * field.
 */
constant_pool_entry_info* add_boolean_field(analyzed_cap_file* acf, class_info* class, u1 flags) {

    field_info** tmp1 = NULL;
    constant_pool_entry_info** tmp2 = NULL;

    field_info* field = NULL;
    constant_pool_entry_info* entry = NULL;

    field = (field_info*)malloc(sizeof(field_info));
    if(field == NULL)
        return NULL;
    memset(field, 0, sizeof(field_info));
    field->flags = flags;

    if(boolean_type == NULL) {
        u2 type_index = acf->signature_pool_count;

        boolean_type = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
        if(boolean_type == NULL)
            return NULL;
        memset(boolean_type, 0, sizeof(type_descriptor_info));
        boolean_type->types_count = 1;
        boolean_type->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * boolean_type->types_count);
        if(boolean_type->types == NULL)
            return NULL;
        memset(boolean_type->types, 0, sizeof(one_type_descriptor_info) * boolean_type->types_count);
        boolean_type->types->type = TYPE_DESCRIPTOR_BOOLEAN;

        if((type_index = is_type_descriptor_not_unique(acf, boolean_type)) == acf->signature_pool_count) {
            type_descriptor_info** tmp3 = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
            if(tmp3 == NULL)
                return NULL;
            acf->signature_pool = tmp3;
            acf->signature_pool[acf->signature_pool_count] = boolean_type;
            ++acf->signature_pool_count;
        } else {
            free(boolean_type->types);
            free(boolean_type);
            boolean_type = acf->signature_pool[type_index];
        }
    }

    field->type = boolean_type;

    entry = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(entry == NULL)
        return NULL;
    memset(entry, 0, sizeof(constant_pool_entry_info));
    if(flags & FIELD_STATIC)
        entry->flags = CONSTANT_POOL_STATICFIELDREF;
    else
        entry->flags = CONSTANT_POOL_INSTANCEFIELDREF;
    entry->type = boolean_type;
    entry->internal_class = class;
    entry->internal_field = field;

    field->this_field = entry;

    tmp1 = (field_info**)realloc(class->fields, sizeof(field_info*) * (class->fields_count + 1));
    if(tmp1 == NULL)
        return NULL;
    class->fields = tmp1;
    class->fields[class->fields_count] = field;
    ++class->fields_count;

    tmp2 = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp2 == NULL)
        return NULL;
    acf->constant_pool = tmp2;
    acf->constant_pool[acf->constant_pool_count] = entry;
    ++acf->constant_pool_count;

    return entry;

}


/**
 * \brief Add a short field to a class including its entries in the signature
 * and constant pools.
 * 
 * \param acf The analyzedi CAP file.
 * \param class The class to which the field is added.
 * \param flags The flags of the new field.
 *
 * \return Return the newly created analyzed constant pool entry to the added
 * field.
 */
constant_pool_entry_info* add_short_field(analyzed_cap_file* acf, class_info* class, u1 flags) {

    field_info** tmp1 = NULL;
    constant_pool_entry_info** tmp2 = NULL;

    field_info* field = NULL;
    constant_pool_entry_info* entry = NULL;

    field = (field_info*)malloc(sizeof(field_info));
    if(field == NULL)
        return NULL;
    memset(field, 0, sizeof(field_info));
    field->flags = flags;

    if(short_type == NULL) {
        u2 type_index = acf->signature_pool_count;

        short_type = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
        if(short_type == NULL)
            return NULL;
        memset(short_type, 0, sizeof(type_descriptor_info));
        short_type->types_count = 1;
        short_type->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * short_type->types_count);
        if(short_type->types == NULL)
            return NULL;
        memset(short_type->types, 0, sizeof(one_type_descriptor_info) * short_type->types_count);
        short_type->types->type = TYPE_DESCRIPTOR_SHORT;

        if((type_index = is_type_descriptor_not_unique(acf, short_type)) == acf->signature_pool_count) {
            type_descriptor_info** tmp3 = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
            if(tmp3 == NULL)
                return NULL;
            acf->signature_pool = tmp3;
            acf->signature_pool[acf->signature_pool_count] = short_type;
            ++acf->signature_pool_count;
        } else {
            free(short_type->types);
            free(short_type);
            short_type = acf->signature_pool[type_index];
        }
    }

    field->type = short_type;

    entry = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(entry == NULL)
        return NULL;
    memset(entry, 0, sizeof(constant_pool_entry_info));
    if(flags & FIELD_STATIC)
        entry->flags = CONSTANT_POOL_STATICFIELDREF;
    else
        entry->flags = CONSTANT_POOL_INSTANCEFIELDREF;

    entry->type = short_type;
    entry->internal_class = class;
    entry->internal_field = field;

    field->this_field = entry;

    tmp1 = (field_info**)realloc(class->fields, sizeof(field_info*) * (class->fields_count + 1));
    if(tmp1 == NULL)
        return NULL;
    class->fields = tmp1;
    class->fields[class->fields_count] = field;
    ++class->fields_count;

    tmp2 = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp2 == NULL)
        return NULL;
    acf->constant_pool = tmp2;
    acf->constant_pool[acf->constant_pool_count] = entry;
    ++acf->constant_pool_count;

    return entry;

}


/**
 * \brief Add a reference field to a class including its entries in the signature
 * and constant pools.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the field is added.
 * \param ref An analyzed constant pool entry to the class type of the
 * reference.
 * \param type An analyzed signature pool entry to the field type. If NULL an
 * analyzed signature pool entry is created.
 * \param flags The flags of the new field.
 *
 * \return Return the newly created analyzed constant pool entry to the added
 * field.
 */
constant_pool_entry_info* add_ref_field(analyzed_cap_file* acf, class_info* class, constant_pool_entry_info* ref, type_descriptor_info** type, u1 flags) {

    field_info** tmp1 = NULL;
    constant_pool_entry_info** tmp2 = NULL;

    field_info* field = NULL;
    constant_pool_entry_info* entry = NULL;

    field = (field_info*)malloc(sizeof(field_info));
    if(field == NULL)
        return NULL;
    memset(field, 0, sizeof(field_info));
    field->flags = flags;

    if(*type == NULL) {
        u2 type_index = acf->signature_pool_count;

        *type = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
        if(*type == NULL)
            return NULL;
        memset(*type, 0, sizeof(type_descriptor_info));
        (*type)->types_count = 1;
        (*type)->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * (*type)->types_count);
        if((*type)->types == NULL)
            return NULL;
        memset((*type)->types, 0, sizeof(one_type_descriptor_info) * (*type)->types_count);
        (*type)->types->type = TYPE_DESCRIPTOR_REF;
        (*type)->types->ref = ref;

        if((type_index = is_type_descriptor_not_unique(acf, *type)) == acf->signature_pool_count) {
            type_descriptor_info** tmp3 = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
            if(tmp3 == NULL)
                return NULL;
            acf->signature_pool = tmp3;
            acf->signature_pool[acf->signature_pool_count] = *type;
            ++acf->signature_pool_count;
        } else {
            free((*type)->types);
            free(*type);
            *type = acf->signature_pool[type_index];
        }
    }

    field->type = *type;

    entry = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(entry == NULL)
        return NULL;
    memset(entry, 0, sizeof(constant_pool_entry_info));
    if(flags & FIELD_STATIC)
        entry->flags = CONSTANT_POOL_STATICFIELDREF;
    else
        entry->flags = CONSTANT_POOL_INSTANCEFIELDREF;
    entry->type = *type;
    entry->internal_class = class;
    entry->internal_field = field;

    field->this_field = entry;

    tmp1 = (field_info**)realloc(class->fields, sizeof(field_info*) * (class->fields_count + 1));
    if(tmp1 == NULL)
        return NULL;
    class->fields = tmp1;
    class->fields[class->fields_count] = field;
    ++class->fields_count;

    tmp2 = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp2 == NULL)
        return NULL;
    acf->constant_pool = tmp2;
    acf->constant_pool[acf->constant_pool_count] = entry;
    ++acf->constant_pool_count;

    return entry;

}
