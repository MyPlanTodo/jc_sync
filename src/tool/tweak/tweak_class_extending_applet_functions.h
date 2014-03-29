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
 * \file tweak_class_extending_applet_functions.h
 */

#include <analyzed_cap_file.h>

#include "javacard_framework_info.h"
#include "synclib_info.h"

/**
 * Tweak the given class while taking into account that it has SyncApplet in its
 * class hierarchy. 
 * Tweak javacard.framework.Applet.getShareableInterfaceObject method if
 * present.
 * Tweak javacard.framework.AppletEvent.uninstall implementing method
 * if present.
 * Tweak javacard.framework.Applet.select method if present.
 * Tweak javacard.framework.Applet.process method if present.
 * Add methods inherited from synclib.SyncApplet if the given class direct
 * superclass is synclib.SyncApplet.
 */
int tweak_class_extending_applet(analyzed_cap_file* acf, class_info* class, javacard_framework_info* javacard_framework, synclib_info* synclib, char is_using_int);

/**
 * Generate the bytecodes for handling static field but also
 * synclib.SyncApplet.getClassId and synclib.SyncApplet.getNewInstance methods.
 * Bytecodes are generated for the following methods:
 * synclib.SyncApplet.getClassId
 * synclib.SyncApplet.getNewInstance
 * synclib.SyncApplet.getNbStaticBooleanArrays
 * synclib.SyncApplet.getAnyStaticBooleanArray
 * synclib.SyncApplet.getNbStaticByteArrays
 * synclib.SyncApplet.getAnyStaticByteArray
 * synclib.SyncApplet.getNbStaticShortArrays
 * synclib.SyncApplet.getAnyStaticShortArray
 * synclib.SyncApplet.getNbStaticIntArrays (if is_using_int is 1)
 * synclib.SyncApplet.getAnyStaticIntArray (if is_using_int is 1)
 * synclib.SyncApplet.resetStaticPathAll
 * synclib.SyncApplet.numberStaticPathAll
 * synclib.SyncApplet.serializeStaticAll
 * synclib.SyncApplet.mergeStaticAll
 */
int generate_bytecodes_handling_static(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib, char is_using_int);
