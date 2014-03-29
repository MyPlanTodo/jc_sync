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
 * \file create_bytecode_functions.h
 */

#include <analyzed_cap_file.h>

bytecode_info* create_aload_0_bytecode(void);
bytecode_info* create_aload_1_bytecode(void);
bytecode_info* create_astore_1_bytecode(void);
bytecode_info* create_sload_0_bytecode(void);
bytecode_info* create_sload_1_bytecode(void);
bytecode_info* create_sload_2_bytecode(void);
bytecode_info* create_sstore_2_bytecode(void);

bytecode_info* create_putfield_T_this_bytecode(constant_pool_entry_info* field_ref);
bytecode_info* create_getfield_T_this_bytecode(constant_pool_entry_info* field_ref);
bytecode_info* create_getfield_T_bytecode(constant_pool_entry_info* field_ref);

bytecode_info* create_putstatic_T_bytecode(constant_pool_entry_info* field_ref);
bytecode_info* create_getstatic_T_bytecode(constant_pool_entry_info* field_ref);

bytecode_info* create_aconst_null_bytecode(void);
bytecode_info* create_push_short_value_bytecode(int16_t value);

bytecode_info* create_return_bytecode(void);
bytecode_info* create_areturn_bytecode(void);
bytecode_info* create_sreturn_bytecode(void);

bytecode_info* create_invokeinterface_bytecode(constant_pool_entry_info* interface_ref, u1 method_token, u1 nargs);
bytecode_info* create_invokespecial_bytecode(constant_pool_entry_info* method_ref);
bytecode_info* create_invokestatic_bytecode(constant_pool_entry_info* method_ref);
bytecode_info* create_invokevirtual_bytecode(constant_pool_entry_info* method_ref);

bytecode_info* create_ifnonnull_bytecode(bytecode_info* branch);
bytecode_info* create_ifne_bytecode(bytecode_info* branch);
bytecode_info* create_ifeq_bytecode(bytecode_info* branch);
bytecode_info* create_if_scmple_bytecode(bytecode_info* branch);
bytecode_info* create_if_scmpeq_bytecode(bytecode_info* branch);
bytecode_info* create_if_scmpne_bytecode(bytecode_info* branch);
bytecode_info* create_if_scmpge_bytecode(bytecode_info* branch);
bytecode_info* create_goto_bytecode(bytecode_info* branch);
bytecode_info* create_stableswitch_bytecode(u2 low, u2 high, bytecode_info** branches, bytecode_info* default_branch);

bytecode_info* create_sadd_bytecode(void);

bytecode_info* create_checkcast_bytecode(constant_pool_entry_info* ref);
bytecode_info* create_instanceof_bytecode(constant_pool_entry_info* ref);
bytecode_info* create_athrow_bytecode(void);

bytecode_info* create_new_bytecode(constant_pool_entry_info* class_ref);

bytecode_info* create_dup_bytecode(void);
bytecode_info* create_pop_bytecode(void);
