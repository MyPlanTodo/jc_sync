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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <analyzed_cap_file.h>


bytecode_info* create_aload_0_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_aload_0_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 24;

    return new_bytecode;

}


bytecode_info* create_aload_1_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_aload_1_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 25;

    return new_bytecode;

}


bytecode_info* create_astore_1_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_astore_1_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 44;

    return new_bytecode;

}


bytecode_info* create_sload_2_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_sload_2_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 30;

    return new_bytecode;

}


bytecode_info* create_sload_1_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_sload_1_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 29;

    return new_bytecode;

}


bytecode_info* create_sload_0_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_sload_0_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 28;

    return new_bytecode;

}


bytecode_info* create_sstore_2_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_sstore_2_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 49;

    return new_bytecode;

}


bytecode_info* create_putfield_T_this_bytecode(constant_pool_entry_info* field_ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_putfield_T_this_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(field_ref->flags & CONSTANT_POOL_INSTANCEFIELDREF)) {
        fprintf(stderr, "The ref is not an instance field ref\n");
        return NULL;
    }

    if(field_ref->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY))
        new_bytecode->opcode = 181;
    else if(field_ref->type->types->type & (TYPE_DESCRIPTOR_BOOLEAN|TYPE_DESCRIPTOR_BYTE))
        new_bytecode->opcode = 182;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_SHORT)
        new_bytecode->opcode = 183;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_INT)
        new_bytecode->opcode = 184;
    else
        return NULL;

    new_bytecode->nb_args = 1;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = field_ref;

    return new_bytecode;

}


bytecode_info* create_getfield_T_this_bytecode(constant_pool_entry_info* field_ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_getfield_T_this_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(field_ref->flags & CONSTANT_POOL_INSTANCEFIELDREF)) {
        fprintf(stderr, "The ref is not an instance field ref\n");
        return NULL;
    }

    if(field_ref->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY))
        new_bytecode->opcode = 173;
    else if(field_ref->type->types->type & (TYPE_DESCRIPTOR_BOOLEAN|TYPE_DESCRIPTOR_BYTE))
        new_bytecode->opcode = 174;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_SHORT)
        new_bytecode->opcode = 175;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_INT)
        new_bytecode->opcode = 176;
    else
        return NULL;

    new_bytecode->nb_args = 1;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = field_ref;

    return new_bytecode;

}


bytecode_info* create_getfield_T_bytecode(constant_pool_entry_info* field_ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_getfield_T_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(field_ref->flags & CONSTANT_POOL_INSTANCEFIELDREF)) {
        fprintf(stderr, "The ref is not an instance field ref\n");
        return NULL;
    }

    if(field_ref->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY))
        new_bytecode->opcode = 131;
    else if(field_ref->type->types->type & (TYPE_DESCRIPTOR_BOOLEAN|TYPE_DESCRIPTOR_BYTE))
        new_bytecode->opcode = 132;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_SHORT)
        new_bytecode->opcode = 133;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_INT)
        new_bytecode->opcode = 134;
    else
        return NULL;

    new_bytecode->nb_args = 1;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = field_ref;

    return new_bytecode;

}


bytecode_info* create_putstatic_T_bytecode(constant_pool_entry_info* field_ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_putstatic_T_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(field_ref->flags & CONSTANT_POOL_STATICFIELDREF)) {
        fprintf(stderr, "The ref is not a static field ref\n");
        return NULL;
    }

    if(field_ref->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY))
        new_bytecode->opcode = 127;
    else if(field_ref->type->types->type & (TYPE_DESCRIPTOR_BOOLEAN|TYPE_DESCRIPTOR_BYTE))
        new_bytecode->opcode = 128;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_SHORT)
        new_bytecode->opcode = 129;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_INT)
        new_bytecode->opcode = 130;
    else
        return NULL;

    new_bytecode->nb_args = 2;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = field_ref;

    return new_bytecode;

}


bytecode_info* create_getstatic_T_bytecode(constant_pool_entry_info* field_ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_getstatic_T_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(field_ref->flags & CONSTANT_POOL_STATICFIELDREF)) {
        fprintf(stderr, "The ref is not a static field ref\n");
        return NULL;
    }

    if(field_ref->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY))
        new_bytecode->opcode = 123;
    else if(field_ref->type->types->type & (TYPE_DESCRIPTOR_BOOLEAN|TYPE_DESCRIPTOR_BYTE))
        new_bytecode->opcode = 124;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_SHORT)
        new_bytecode->opcode = 125;
    else if(field_ref->type->types->type & TYPE_DESCRIPTOR_INT)
        new_bytecode->opcode = 126;
    else
        return NULL;

    new_bytecode->nb_args = 2;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = field_ref;

    return new_bytecode;

}


bytecode_info* create_aconst_null_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_aconst_null_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 1;

    return new_bytecode;

}


bytecode_info* create_push_short_value_bytecode(int16_t value) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_push_short_value_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    switch(value) {
        case -1:
            new_bytecode->opcode = 2; // sconst_m1
            break;

        case 0:
            new_bytecode->opcode = 3; // sconst_0
            break;

        case 1:
            new_bytecode->opcode = 4; // sconst_1
            break;

        case 2:
            new_bytecode->opcode = 5; // sconst_2
            break;

        case 3:
            new_bytecode->opcode = 6; // sconst_3
            break;

        case 4:
            new_bytecode->opcode = 7; // sconst_4
            break;

        case 5:
            new_bytecode->opcode = 8; // sconst_5
            break;

        default:
           new_bytecode->opcode = 17; // sspush
           new_bytecode->nb_args = 2;
           new_bytecode->nb_byte_args = 2;
           new_bytecode->args[0] = (u1)((value >> 8) & 0xFF);
           new_bytecode->args[1] = (u1)(value & 0xFF);
    }

    return new_bytecode;

}


bytecode_info* create_return_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_return_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 122;

    return new_bytecode;

}


bytecode_info* create_areturn_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_areturn_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 119;

    return new_bytecode;

}


bytecode_info* create_sreturn_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_sreturn_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 120;

    return new_bytecode;

}


bytecode_info* create_invokeinterface_bytecode(constant_pool_entry_info* interface_ref, u1 method_token, u1 nargs) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_invokeinterface_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(interface_ref->flags & CONSTANT_POOL_CLASSREF)) {
        fprintf(stderr, "The ref is not a class ref\n");
        assert(0);
        return NULL;
    }

    new_bytecode->opcode = 142;
    new_bytecode->nb_args = 4;
    new_bytecode->nb_byte_args = 2;
    new_bytecode->args[0] = nargs;
    new_bytecode->args[1] = method_token;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = interface_ref;

    return new_bytecode;

}


bytecode_info* create_invokespecial_bytecode(constant_pool_entry_info* method_ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_invokespecial_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(method_ref->flags & (CONSTANT_POOL_STATICMETHODREF|CONSTANT_POOL_SUPERMETHODREF))) {
        fprintf(stderr, "The ref is not a static method or a super method ref\n");
        return NULL;
    }

    new_bytecode->opcode = 140;
    new_bytecode->nb_args = 2;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = method_ref;

    return new_bytecode;

}


bytecode_info* create_invokestatic_bytecode(constant_pool_entry_info* method_ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_invokestatic_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(method_ref->flags & CONSTANT_POOL_STATICMETHODREF)) {
        fprintf(stderr, "The ref is not a static method ref\n");
        return NULL;
    }

    new_bytecode->opcode = 141;
    new_bytecode->nb_args = 2;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = method_ref;

    return new_bytecode;

}


bytecode_info* create_invokevirtual_bytecode(constant_pool_entry_info* method_ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_invokevirtual_bytecode");
        return NULL;
    }

    if(!(method_ref->flags & CONSTANT_POOL_VIRTUALMETHODREF)) {
        fprintf(stderr, "The ref is not a virtual method ref\n");
        return NULL;
    }

    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 139;
    new_bytecode->nb_args = 2;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = method_ref;

    return new_bytecode;

}


bytecode_info* create_ifne_bytecode(bytecode_info* branch) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_ifne_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 97;
    new_bytecode->nb_args = 1;
    new_bytecode->has_branch = 1;
    new_bytecode->branch = branch;

    return new_bytecode;

}


bytecode_info* create_ifnonnull_bytecode(bytecode_info* branch) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_ifnonnull_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 103;
    new_bytecode->nb_args = 1;
    new_bytecode->has_branch = 1;
    new_bytecode->branch = branch;

    return new_bytecode;

}


bytecode_info* create_ifeq_bytecode(bytecode_info* branch) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_ifeq_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 96;
    new_bytecode->nb_args = 1;
    new_bytecode->has_branch = 1;
    new_bytecode->branch = branch;

    return new_bytecode;

}


bytecode_info* create_if_scmple_bytecode(bytecode_info* branch) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_if_scmple_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 111;
    new_bytecode->nb_args = 1;
    new_bytecode->has_branch = 1;
    new_bytecode->branch = branch;

    return new_bytecode;

}


bytecode_info* create_if_scmpeq_bytecode(bytecode_info* branch) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_if_scmpeq_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 106;
    new_bytecode->nb_args = 1;
    new_bytecode->has_branch = 1;
    new_bytecode->branch = branch;

    return new_bytecode;

}


bytecode_info* create_if_scmpne_bytecode(bytecode_info* branch) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_if_scmpne_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 107;
    new_bytecode->nb_args = 1;
    new_bytecode->has_branch = 1;
    new_bytecode->branch = branch;

    return new_bytecode;

}


bytecode_info* create_if_scmpge_bytecode(bytecode_info* branch) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_if_scmpge_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 109;
    new_bytecode->nb_args = 1;
    new_bytecode->has_branch = 1;
    new_bytecode->branch = branch;

    return new_bytecode;

}


bytecode_info* create_goto_bytecode(bytecode_info* branch) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_goto_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 112;
    new_bytecode->nb_args = 1;
    new_bytecode->has_branch = 1;
    new_bytecode->branch = branch;

    return new_bytecode;

}


bytecode_info* create_stableswitch_bytecode(u2 low, u2 high, bytecode_info** branches, bytecode_info* default_branch) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_stableswitch_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 115;
    new_bytecode->nb_args = 6 + (2 * (high - low + 1));
    new_bytecode->stableswitch.nb_cases = high - low + 1;
    new_bytecode->stableswitch.low = low;
    new_bytecode->stableswitch.high = high;
    new_bytecode->stableswitch.default_branch = default_branch;
    new_bytecode->stableswitch.branches = branches;

    return new_bytecode;

}


bytecode_info* create_sadd_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_sadd_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 65;

    return new_bytecode;

}


bytecode_info* create_checkcast_bytecode(constant_pool_entry_info* ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_checkcast_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(ref->flags & CONSTANT_POOL_CLASSREF)) {
        fprintf(stderr, "The ref is not a class ref\n");
        assert(0);
        return NULL;
    }

    new_bytecode->opcode = 148;
    new_bytecode->nb_args = 3;
    new_bytecode->nb_byte_args = 1;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = ref;

    return new_bytecode;

}


bytecode_info* create_instanceof_bytecode(constant_pool_entry_info* ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_instanceof_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(ref->flags & CONSTANT_POOL_CLASSREF)) {
        fprintf(stderr, "The ref is not a class ref\n");
        assert(0);
        return NULL;
    }

    new_bytecode->opcode = 149;
    new_bytecode->nb_args = 3;
    new_bytecode->nb_byte_args = 1;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = ref;

    return new_bytecode;

}


bytecode_info* create_athrow_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_athrow_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 147;

    return new_bytecode;

}


bytecode_info* create_new_bytecode(constant_pool_entry_info* class_ref) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_new_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));

    if(!(class_ref->flags & CONSTANT_POOL_CLASSREF)) {
        fprintf(stderr, "The ref is not a class ref\n");
        assert(0);
        return NULL;
    }

    new_bytecode->opcode = 143;
    new_bytecode->nb_args = 2;
    new_bytecode->has_ref = 1;
    new_bytecode->ref = class_ref;

    return new_bytecode;

}


bytecode_info* create_dup_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_dup_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 61;

    return new_bytecode;

}

bytecode_info* create_pop_bytecode(void) {

    bytecode_info* new_bytecode = (bytecode_info*)malloc(sizeof(bytecode_info));
    if(new_bytecode == NULL) {
        perror("create_pop_bytecode");
        return NULL;
    }
    memset(new_bytecode, 0, sizeof(bytecode_info));
    new_bytecode->opcode = 59;

    return new_bytecode;

}
