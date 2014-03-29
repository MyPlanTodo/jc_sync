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
 * \file synchronizable_itf_functions.h
 */

#include <analyzed_cap_file.h>

#include "javacard_framework_info.h"
#include "synclib_info.h"

/**
 * Add the Synchronizable interface to the given class, add implementing methods
 * if needed and add supporting fields if needed.
 *
 * If the given class has synclib.SyncApplet as direct superclass or if its direct
 * superclass does not implement Synchronizable or if it does not have a
 * superclass, the short id field, the boolean isDuplicated field and the
 * Synchronizable previous field are added.
 *
 * If the given class has more than one instance field or has at least an instance array
 * field or an instance reference field or has a Synchronizable superclass
 * (except synclib.SyncApplet) and at least one instance field, the short
 * currentField field is added.
 *
 * Methods are added if needed and override inherited methods if needed.
 * Needed super_methodref constant pool entries are also added if needed.
 * Bytecodes are generated for each added method.
 */
int add_Synchronizable_interface(analyzed_cap_file* acf, class_info* class, javacard_framework_info* javacard_framework, synclib_info* synclib);
