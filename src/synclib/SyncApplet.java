/*
 * Copyright Inria:
 * Jean-François Hren
 * 
 * jfhren[at]gmail[dot]com
 * michael[dot]hauspie[at]lifl[dot]com
 * 
 * This software is a computer program whose purpose is to synchronize
 * Java Card 2 applet data.
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

package synclib;

import javacard.framework.AID;
import javacard.framework.APDU;
import javacard.framework.Applet;
import javacard.framework.AppletEvent;
import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.JCSystem;
import javacard.framework.Shareable;
import javacard.framework.UserException;

/**
 * This class makes the link between an applet extending it and the SyncManager.
 */
public abstract class SyncApplet extends Applet implements AppletEvent, Synchronizable, SyncSynchronizable {

    /**
     * The maximum number of duplicated Synchronizable instances.
     */
    private final static short maxNbDuplicatedInstances = (short)128;

    /**
     * The AID of the SyncManager applet.
     */
    private final byte[] SYNC_AID = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF, (byte)0x00, (byte)0x01};

    /**
     * A reference to the instance of the SyncManager AID owned by the VM.
     */
    protected static AID syncAID = null;

    /**
     * An output sink used for the serialization of the applet.
     */
    private Output out = null;

    /**
     * An input source used for the merging of the applet.
     */
    private Input in = null;

    /**
     * Is the applet not in a broken state? (Partial merge)
     */
    private boolean isReady = true;

    /**
     * The Synchronizable instance being currently serialized or merged.
     */
    private static Synchronizable currentInstance = null;

    /**
     * Is the static fields serialization or merge done?
     */
    private static boolean isStaticDone = false;

    /**
     * Should the current Synchronizable instance be serialized or merged?
     */
    private static boolean shouldSerializeOrMerge = false;

    /**
     * Array of duplicated Synchronizable instances.
     */
    private DuplicatedInstance[] duplicatedInstances = null;

    /**
     * The number of duplicated Synchronizable instances.
     * @see #duplicatedInstances
     */
    private static short nbDuplicatedInstances = (short)0;

    /**
     * Linked list of instances used for finding duplicate primitive array references.
     */
    private static SynchronizableArrayFields instancesLinkedList = null;

    /**
     * If a duplicate primitive array reference was found, its field id is saved up here.
     * @see #isArrayReferenceDuplicate(boolean[], short)
     * @see #isArrayReferenceDuplicate(byte[], short)
     * @see #isArrayReferenceDuplicate(short[], short)
     */
    static short duplicatedArrayReferenceFieldId = (short)0;

    /**
     * If a duplicate primitive array reference was found, its instance id is saved up here.
     * @see #isArrayReferenceDuplicate(boolean[], short)
     * @see #isArrayReferenceDuplicate(byte[], short)
     * @see #isArrayReferenceDuplicate(short[], short)
     */
    static short duplicatedArrayReferenceInstanceId = (short)0;


    /**
     * Called when a child class is instanced.
     * Fetch a reference to the instance of the SyncManager AID if not done before and instanciate the output and input sinks.
     */
    protected SyncApplet() {

        if(syncAID == null)
            syncAID = JCSystem.lookupAID(SYNC_AID, (short)0, (byte)0x0b);

    }


    /**
     * Register the applet whitin sync using a shareable interface.
     */
    protected final void syncRegister() {

        register();
        SyncService aSyncService = (SyncService)JCSystem.getAppletShareableInterfaceObject(syncAID, (byte)0);
        aSyncService.register();

        out = new Output(this);
        in = new Input(this);
        duplicatedInstances = new DuplicatedInstance[maxNbDuplicatedInstances];

    }


    /**
     * Register the applet whitin sync using a shareable interface.
     * @param bArray the byte array containing the AID bytes
     * @param bOffset the start of AID bytes in bArray
     * @param bLength the length of the AID bytes in bArray
     */
    protected final void syncRegister(byte[] bArray, short bOffset, byte bLength) {

        register(bArray, bOffset, bLength);
        SyncService aSyncService = (SyncService)JCSystem.getAppletShareableInterfaceObject(syncAID, (byte)0);
        aSyncService.register();

        out = new Output(this);
        in = new Input(this);
        duplicatedInstances = new DuplicatedInstance[maxNbDuplicatedInstances];

    }


    /**
     * Check if this applet is ready to process APDU.
     * @throws ISOException An ISOException exception is thrown if the applet is not ready.
     * @see SyncSW#INCOMPLETE_MERGE
     */
    private final void checkReadiness()
        throws ISOException {

        if(!isReady)
            ISOException.throwIt(SyncSW.INCOMPLETE_MERGE);

    }

    public void process(APDU apdu)
        throws ISOException {

        checkReadiness();

    }

    public boolean select() {

        checkReadiness();
        return true;

    }


    /**
     * Return the number of static boolean arrays in this package classes.
     * @return Return the number of static boolean arrays.
     */
    protected short getNbStaticBooleanArrays() {
        return 0;
    }

    /**
     * Return a boolean array given its static field id in this package classes.
     * @param fieldId The id of the boolean array.
     * @return Return the boolean array.
     * @throws ISOException An ISOException exception is thrown if the field id is invalid.
     * @see ISO7816#SW_UNKNOWN
     */
    protected boolean[] getAnyStaticBooleanArray(short fieldId) {
        ISOException.throwIt(ISO7816.SW_UNKNOWN);
        return null;
    }

    /**
     * Return the number of static byte arrays in this package classes.
     * @return Return the number of static byte arrays.
     */
    protected short getNbStaticByteArrays() {
        return 0;
    }

    /**
     * Return a byte array given its static field id in this package classes.
     * @param fieldId The id of the byte array.
     * @return Return the byte array.
     * @throws ISOException An ISOException exception is thrown if the field id is invalid.
     * @see ISO7816#SW_UNKNOWN
     */
    protected byte[] getAnyStaticByteArray(short fieldId) {
        ISOException.throwIt(ISO7816.SW_UNKNOWN);
        return null;
    }

    /**
     * Return the number of static short arrays in this package classes.
     * @return Return the number of static short arrays.
     */
    protected short getNbStaticShortArrays() {
        return 0;
    }

    /**
     * Return a short array given its static field id in this package classes.
     * @param fieldId The id of the short array.
     * @return Return the short array.
     * @throws ISOException An ISOException exception is thrown if the field id is invalid.
     * @see ISO7816#SW_UNKNOWN
     */
    protected short[] getAnyStaticShortArray(short fieldId) {
        ISOException.throwIt(ISO7816.SW_UNKNOWN);
        return null;
    }


    /**
     * Search for a duplicate boolean array reference in the set of static boolean array of this package classes and in the instances linked list.
     * @param searchedArray The reference of the boolean array being searched for.
     * @param fieldId The id of the field related to the boolean array.
     * @return Return true if the given boolean array reference is a duplicate. The original field id and instance id are saved up for later use. Return false else.
     * @see #duplicatedArrayReferenceFieldId
     * @see #duplicatedArrayReferenceInstanceId
     */
    boolean isArrayReferenceDuplicate(boolean[] searchedArray, short fieldId) {

        if(!isStaticDone) {
            for(short i = (short)0; i < fieldId; ++i)
                if(this.getAnyStaticBooleanArray(i) == searchedArray) {
                    duplicatedArrayReferenceFieldId = i;
                    duplicatedArrayReferenceInstanceId = (short)-1;
                }
        } else {
            short nbBooleanArrays = this.getNbStaticBooleanArrays();

            for(short i = (short)0; i < nbBooleanArrays; ++i)
                if(this.getAnyStaticBooleanArray(i) == searchedArray) {
                    duplicatedArrayReferenceFieldId = i;
                    duplicatedArrayReferenceInstanceId = (short)-1;
                    return true;
                }

            SynchronizableArrayFields currentInstance = instancesLinkedList;

            for(short i = (short)0; i < fieldId; ++i)
                if(currentInstance.getBooleanArray(i) == searchedArray) {
                    duplicatedArrayReferenceFieldId = i;
                    duplicatedArrayReferenceInstanceId = currentInstance.getId();
                    return true;
                }

            currentInstance = currentInstance.getNext();

            while(currentInstance != null) {
                nbBooleanArrays = currentInstance.getNbBooleanArrays();

                for(short i = (short)0; i < nbBooleanArrays; ++i)
                    if(currentInstance.getBooleanArray(i) == searchedArray) {
                        duplicatedArrayReferenceFieldId = i;
                        duplicatedArrayReferenceInstanceId = currentInstance.getId();
                        return true;
                    }
                currentInstance = currentInstance.getNext();
            }
        }

        return false;

    }


    /**
     * Search for a duplicate byte array reference in the set of static byte array of this package classes and in the instances linked list.
     * @param searchedArray The reference of the byte array being searched for.
     * @param fieldId The id of the field related to the byte array.
     * @return Return true if the given byte array reference is a duplicate. The original field id and instance id are saved up for later use. Return false else.
     * @see #duplicatedArrayReferenceFieldId
     * @see #duplicatedArrayReferenceInstanceId
     */
    boolean isArrayReferenceDuplicate(byte[] searchedArray, short fieldId) {

        if(!isStaticDone) {
            for(short i = (short)0; i < fieldId; ++i)
                if(this.getAnyStaticByteArray(i) == searchedArray) {
                    duplicatedArrayReferenceFieldId = i;
                    duplicatedArrayReferenceInstanceId = (short)-1;
                }
        } else {
            short nbByteArrays = this.getNbStaticByteArrays();

            for(short i = (short)0; i < nbByteArrays; ++i)
                if((this.getAnyStaticByteArray(i) == searchedArray) && (isStaticDone || (i != fieldId))) {
                    duplicatedArrayReferenceFieldId = i;
                    duplicatedArrayReferenceInstanceId = (short)-1;
                    return true;
                }

            SynchronizableArrayFields currentInstance = instancesLinkedList;

            for(short i = (short)0; i < fieldId; ++i)
                if(currentInstance.getByteArray(i) == searchedArray) {
                    duplicatedArrayReferenceFieldId = i;
                    duplicatedArrayReferenceInstanceId = currentInstance.getId();
                    return true;
                }

            currentInstance = currentInstance.getNext();

            while(currentInstance != null) {
                nbByteArrays = currentInstance.getNbByteArrays();

                for(short i = (short)0; i < nbByteArrays; ++i)
                    if(currentInstance.getByteArray(i) == searchedArray) {
                        duplicatedArrayReferenceFieldId = i;
                        duplicatedArrayReferenceInstanceId = currentInstance.getId();
                        return true;
                    }
                currentInstance = currentInstance.getNext();
            }
        }

        return false;

    }


    /**
     * Search for a duplicate short array reference in the set of static short array of this package classes and in the instances linked list.
     * @param searchedArray The reference of the short array being searched for.
     * @param fieldId The id of the field related to the short array.
     * @return Return true if the given short array reference is a duplicate. The original field id and instance id are saved up for later use. Return false else.
     * @see #duplicatedArrayReferenceFieldId
     * @see #duplicatedArrayReferenceInstanceId
     */
    boolean isArrayReferenceDuplicate(short[] searchedArray, short fieldId) {

        if(!isStaticDone) {
            for(short i = (short)0; i < fieldId; ++i)
                if(this.getAnyStaticShortArray(i) == searchedArray) {
                    duplicatedArrayReferenceFieldId = i;
                    duplicatedArrayReferenceInstanceId = (short)-1;
                }
        } else {
            short nbShortArrays = this.getNbStaticShortArrays();

            for(short i = (short)0; i < nbShortArrays; ++i)
                if((this.getAnyStaticShortArray(i) == searchedArray) && (isStaticDone || (i != fieldId))) {
                    duplicatedArrayReferenceFieldId = i;
                    duplicatedArrayReferenceInstanceId = (short)-1;
                    return true;
                }

            SynchronizableArrayFields currentInstance = instancesLinkedList;

            for(short i = (short)0; i < fieldId; ++i)
                if(currentInstance.getShortArray(i) == searchedArray) {
                    duplicatedArrayReferenceFieldId = i;
                    duplicatedArrayReferenceInstanceId = currentInstance.getId();
                    return true;
                }

            currentInstance = currentInstance.getNext();

            while(currentInstance != null) {
                nbShortArrays = currentInstance.getNbShortArrays();

                for(short i = (short)0; i < nbShortArrays; ++i)
                    if(currentInstance.getShortArray(i) == searchedArray) {
                        duplicatedArrayReferenceFieldId = i;
                        duplicatedArrayReferenceInstanceId = currentInstance.getId();
                        return true;
                    }
                currentInstance = currentInstance.getNext();
            }
        }

        return false;

    }


    /**
     * Get a duplicate boolean array reference in the set of static boolean arrays or in the instances linked list.
     * @param instanceId The id of the instance in which the boolean array reference is. -1 if the reference is in a static field.
     * @param fieldId The id of the field.
     * @return Return the reference of the duplicated boolean array.
     * @throws ISOException An ISOException exception is thrown if the boolean array reference is not found.
     * @see SyncApplet#getAnyStaticBooleanArray(short)
     * @see SynchronizableArrayFields#getBooleanArray(short)
     * @see ISO7816#SW_UNKNOWN
     */
    boolean[] getDuplicatedBooleanArray(short instanceId, short fieldId)
        throws ISOException {

        if(instanceId == (short)-1) {
            return this.getAnyStaticBooleanArray(fieldId);
        } else {
            SynchronizableArrayFields currentInstance = instancesLinkedList;
            while(currentInstance != null) {
                if(currentInstance.getId() == instanceId)
                    return currentInstance.getBooleanArray(fieldId);
                currentInstance = currentInstance.getNext();
            }

            ISOException.throwIt(ISO7816.SW_UNKNOWN);
        }

        return null;

    }


    /**
     * Get a duplicate byte array reference in the set of static byte arrays or in the instances linked list.
     * @param instanceId The id of the instance in which the byte array reference is. -1 if the reference is in a static field.
     * @param fieldId The id of the field.
     * @return Return the reference of the duplicated byte array.
     * @throws ISOException An ISOException exception is thrown if the byte array reference is not found.
     * @see SyncApplet#getAnyStaticByteArray(short)
     * @see SynchronizableArrayFields#getByteArray(short)
     * @see ISO7816#SW_UNKNOWN
     */
    byte[] getDuplicatedByteArray(short instanceId, short fieldId)
        throws ISOException {

        if(instanceId == (short)-1) {
            return this.getAnyStaticByteArray(fieldId);
        } else {
            SynchronizableArrayFields currentInstance = instancesLinkedList;
            while(currentInstance != null) {
                if(currentInstance.getId() == instanceId)
                    return currentInstance.getByteArray(fieldId);
                currentInstance = currentInstance.getNext();
            }

            ISOException.throwIt(ISO7816.SW_UNKNOWN);
        }

        return null;

    }


    /**
     * Get a duplicate short array reference in the set of static short arrays or in the instances linked list.
     * @param instanceId The id of the instance in which the short array reference is. -1 if the reference is in a static field.
     * @param fieldId The id of the field.
     * @return Return the reference of the duplicated short array.
     * @throws ISOException An ISOException exception is thrown if the boolean array reference is not found.
     * @see SyncApplet#getAnyStaticShortArray(short)
     * @see SynchronizableArrayFields#getShortArray(short)
     * @see ISO7816#SW_UNKNOWN
     */
    short[] getDuplicatedShortArray(short instanceId, short fieldId)
        throws ISOException {

        if(instanceId == (short)-1) {
            return this.getAnyStaticShortArray(fieldId);
        } else {
            SynchronizableArrayFields currentInstance = instancesLinkedList;
            while(currentInstance != null) {
                if(currentInstance.getId() == instanceId)
                    return currentInstance.getShortArray(fieldId);
                currentInstance = currentInstance.getNext();
            }

            ISOException.throwIt(ISO7816.SW_UNKNOWN);
        }

        return null;

    }

    /**
     * Add a duplicated Synchronizable instance to the current list for future reuse.
     * @param id The id of the added duplicated Synchronizable instance.
     * @param instance The duplicated Synchronizable instance to add.
     * @throws ISOException An ISOException exception is thrown if the underlying storage is full.
     * @see ISO7816#SW_UNKNOWN
     */
    private void addDuplicatedInstance(short id, Synchronizable instance)
        throws ISOException {

        for(short i = (short)0; i < nbDuplicatedInstances; ++i)
            if(duplicatedInstances[i].instanceId == id)
                return;

        if(nbDuplicatedInstances == maxNbDuplicatedInstances)
            ISOException.throwIt(ISO7816.SW_UNKNOWN);

        if(duplicatedInstances[nbDuplicatedInstances] == null)
            duplicatedInstances[nbDuplicatedInstances] = new DuplicatedInstance();
        duplicatedInstances[nbDuplicatedInstances].instanceId = id;
        duplicatedInstances[nbDuplicatedInstances].instance = instance;
        ++nbDuplicatedInstances;

    }


    /**
     * Get a duplicated Synchronizable instance by its id.
     * @param id The duplicated Synchronizable instance id.
     * @return Return the duplicated Synchronizable instance.
     * @throws ISOException An ISOException is thrown if the duplicate Synchronizable instance is not found.
     * @see ISO7816#SW_UNKNOWN
     */
    private Synchronizable getDuplicatedInstance(short id)
        throws ISOException {

        for(short i = (short)0; i < nbDuplicatedInstances; ++i)
            if(duplicatedInstances[i].instanceId == id)
                return duplicatedInstances[i].instance;

        ISOException.throwIt(ISO7816.SW_UNKNOWN);
        return null;

    }


    /**
     * Return the id of this Synchronizable instance.
     * @return Return the id.
     */
    public abstract short getId();


    /**
     * Return the duplicate state.
     * @return Return true if this Synchronizable instance is duplicated, false else.
     */
    public abstract boolean isDuplicated();


    /**
     * Set the previously visited Synchronizable instance for later backtracking.
     * @param previous The previously visited Synchronizable instance.
     */
    public abstract void setPreviousSynchronizableInstance(Synchronizable previous);


    /**
     * Get the previously visited Synchronizable instance for backtracking to it.
     * @return Return the previously visited Synchronizable instance.
     */
    public abstract Synchronizable getPreviousSynchronizableInstance();


    /**
     * Give the class id of the given Synchronizable instance.
     * @param instance The instance from which we want the uppermost class id.
     * @return Return the class id.
     */
    protected short getClassId(Synchronizable instance) {
        return -1;
    }


    /**
     * Give a new instance given a class id.
     * @param in Passed down to the class constructor.
     * @param classId The class id of the new instance.
     * @return Return a new instance with the given class id.
     */
    protected Synchronizable getNewInstance(Input in, short classId) {
        return null;
    }


    /**
     * Reset the path supporting fields to their default values.
     * @return Return true if this Synchronizable instance fields were reset, false else.
     */
    public abstract boolean resetPath(ResetIdSource source);


    /**
     * Entry point for reseting the path supporting fields to their default values in the static Synchronizable instance fields and Synchronizable instances with static fields.
     * @param source A source to get the current reset value for the id.
     */
    protected void resetStaticPathAll(ResetIdSource source) {
        return;
    }


    /**
     * Iterate over all reachable Synchronizable instances to reset them.
     * @param source A source to get the current reset for the id.
     * @param currentInstance The starting point for the iterative process.
     */
    protected final static void resetPathAll(ResetIdSource source, Synchronizable currentInstance) {

        if(currentInstance.resetPath(source)) {
            currentInstance.setPreviousSynchronizableInstance(null);   //To be sure it is the root

            if(currentInstance instanceof SynchronizableInstanceFields) {
                do {
                    if(((SynchronizableInstanceFields)currentInstance).hasCurrentSynchronizableInstance()) {
                        Synchronizable nextInstance = null;

                        try {
                            nextInstance = ((SynchronizableInstanceFields)currentInstance).getCurrentSynchronizableInstance();
                        }
                        catch(ClassCastException e) {
                        }

                        ((SynchronizableInstanceFields)currentInstance).selectNextSynchronizableInstance();

                        if(nextInstance != null) {
                            if(nextInstance.resetPath(source)) {
                                if(nextInstance instanceof SynchronizableInstanceFields) {
                                    nextInstance.setPreviousSynchronizableInstance(currentInstance);
                                    currentInstance = nextInstance;
                                }
                            }
                        }
                    } else {
                        ((SynchronizableInstanceFields)currentInstance).resetNextSynchronizableInstanceSelect();
                        currentInstance = currentInstance.getPreviousSynchronizableInstance();
                    }
                } while(currentInstance != null);
            }
        }

    }


    /**
     * Number an instance with an id for duplicate instances detection.
     * @return Return true if this Synchronizable instance was numbered, false else.
     */
    public abstract boolean numberPath(Output out);


    /**
     * Entry point for numbering the Synchronizable instances referenced by a static field.
     * @param out Provides ids.
     */
    protected void numberStaticPathAll(Output out) {
        return;
    }


    /**
     * Iterate over all reachable Synchronizable instances to number them.
     * @param out Provides ids.
     * @param currentInstance The starting point for the iterative process.
     */
    protected final static void numberPathAll(Output out, Synchronizable currentInstance) {

        if(currentInstance.numberPath(out)) {
            currentInstance.setPreviousSynchronizableInstance(null);

            if(currentInstance instanceof SynchronizableInstanceFields) {
                do {
                    if(((SynchronizableInstanceFields)currentInstance).hasCurrentSynchronizableInstance()) {
                        Synchronizable nextInstance = null;
                        try {
                            nextInstance = ((SynchronizableInstanceFields)currentInstance).getCurrentSynchronizableInstance();
                        }
                        catch(ClassCastException e) {
                        }
                        ((SynchronizableInstanceFields)currentInstance).selectNextSynchronizableInstance();

                        if(nextInstance != null) {
                            if(nextInstance.numberPath(out)) {
                                if(nextInstance instanceof SynchronizableInstanceFields) {
                                    nextInstance.setPreviousSynchronizableInstance(currentInstance);
                                    currentInstance = nextInstance;
                                }
                            }
                        }
                    } else {
                        ((SynchronizableInstanceFields)currentInstance).resetNextSynchronizableInstanceSelect();
                        currentInstance = currentInstance.getPreviousSynchronizableInstance();
                    }
                } while(currentInstance != null);
            }
        }

    }


    /**
     * Serialize primitive type and array of primitive type fields.
     * @param out The sink for the output.
     * @throws ISOException An ISOException exception can be thrown be a write method of Output.
     * @throws UserException An UserException exception can be thrown be a write method of Output.
     * @see Output#write(boolean)
     * @see Output#write(byte)
     * @see Output#write(short)
     * @see Output#write(boolean[], short)
     * @see Output#write(byte[], short)
     * @see Output#write(short[], short)
     */
    public abstract void serialize(Output out) throws ISOException, UserException;


    /**
     * Entry point for serializating the static fields of Synchronizable instances.
     * @param out The sink for the output.
     * @see #serialize(Output)
     */
    protected void serializeStaticAll(Output out)
        throws ISOException, UserException {
        return;
    }


    /**
     * Iterate over all reacheable Synchronizable instances to serialize them.
     * @param out The sink for the output.
     * @param root The starting point for the iterative process.
     * @throws ISOException An ISOException can be thrown by {@link Output#startInstanceSerialization(Synchronizable)} or {@link #serialize(Output)}.
     * @throws UserException An UserException can be thrown by {@link Output#startInstanceSerialization(Synchronizable)} or {@link #serialize(Output)}.
     */
    protected final static void serializeAll(Output out, Synchronizable root)
        throws ISOException, UserException {

        if(root == null) {
            out.startInstanceSerialization(null);
//            out.endInstanceSerialization(null);
            out.resetResume();
            return;
        }

        if(currentInstance == null) {   // If it is our first time here, we set up the currentInstance
            out.startInstanceSerialization(root);
            out.resetResume();

            if(out.isDuplicate(root.getId()))
                return;

            root.setPreviousSynchronizableInstance(null);

            if(root instanceof SynchronizableArrayFields) {
                ((SynchronizableArrayFields)root).setNext(instancesLinkedList);
                instancesLinkedList = (SynchronizableArrayFields)root;
            }

            currentInstance = root;
            shouldSerializeOrMerge = true;
        }

        do {
            if(shouldSerializeOrMerge) {
                currentInstance.serialize(out);
                out.endInstanceSerialization(currentInstance);
                out.resetResume();
                shouldSerializeOrMerge = false;
            }

            if(currentInstance instanceof SynchronizableInstanceFields) {
                SynchronizableInstanceFields currentFather = (SynchronizableInstanceFields)currentInstance;

                if(currentFather.hasCurrentSynchronizableInstance()) {
                    Synchronizable nextInstance = null;
                    try {
                        nextInstance = currentFather.getCurrentSynchronizableInstance();
                    }
                    catch(ClassCastException e) {
                        out.skipInstance();
                        out.resetResume();
                        currentFather.selectNextSynchronizableInstance();
                        continue;
                    }
                    out.startInstanceSerialization(nextInstance);
                    out.resetResume();

                    currentFather.selectNextSynchronizableInstance();

                    if(nextInstance != null) {
                        if(out.isDuplicate(nextInstance.getId()))
                            continue;

                        nextInstance.setPreviousSynchronizableInstance(currentInstance);

                        if(nextInstance instanceof SynchronizableArrayFields) {
                            ((SynchronizableArrayFields)nextInstance).setNext(instancesLinkedList);
                            instancesLinkedList = (SynchronizableArrayFields)nextInstance;
                        }

                        currentInstance = nextInstance;
                        shouldSerializeOrMerge = true;
                    }
                } else {
                    currentFather.resetNextSynchronizableInstanceSelect();
                    currentInstance = currentInstance.getPreviousSynchronizableInstance();
                }
            } else {
                currentInstance = currentInstance.getPreviousSynchronizableInstance();
            }
        } while(currentInstance != null);

    }


    /**
     * This method is called by the SyncManager to start the serialize process.
     * @param shouldResume If true, the serialize process is resumed else it's started.
     * @param shouldReplay If true, the serialize process is resumed at the previous starting position else it's started.
     * @throws ISOException An ISOException is thrown if the caller is not the SyncManager.
     * @throws UserException An UserException is thrown when the write process is interrupted.
     */
    public final void syncSerialize(boolean shouldResume, boolean shouldReplay)
        throws ISOException, UserException {

        if(!JCSystem.getPreviousContextAID().equals(syncAID))
            ISOException.throwIt(SyncSW.IS_NOT_sync);

        if(!isReady)
            ISOException.throwIt(SyncSW.INCOMPLETE_MERGE);

        if(shouldResume) {
            out.resume();
        } else if(shouldReplay) {
            out.replay();
        } else {
            SyncOutput syncOut = (SyncOutput)JCSystem.getAppletShareableInterfaceObject(syncAID, (byte)1);
            out.start(syncOut);

            currentInstance = null;
            isStaticDone = false;
            nbDuplicatedInstances = (short)0;
            instancesLinkedList = null;

            this.resetStaticPathAll(out);
            resetPathAll(out, this);

            this.numberStaticPathAll(out);
            numberPathAll(out, this);

            out.resetId();
        }

        if(!isStaticDone) {
            this.serializeStaticAll(out);
            out.resetResume();
            currentInstance = null;
            isStaticDone = true;
        }

        serializeAll(out, this);

        out.done();

    }


    /**
     * Merge primitive type and array of primitive type fields.
     * @param in The source for the input.
     * @throws ISOException An ISOException exception can be thrown be a read or checkArray method of Input.
     * @throws UserException An UserException exception can be thrown be a read or checkArray method of Input.
     * @see Input#read(boolean)
     * @see Input#read(byte)
     * @see Input#read(short)
     * @see Input#checkArray(boolean[])
     * @see Input#read(boolean[])
     * @see Input#checkArray(byte[])
     * @see Input#read(byte[])
     * @see Input#checkArray(short[])
     * @see Input#read(short[])
     * @see Input#readN(short)
     * @see Input#getReadNValue(short)
     */
    public abstract void merge(Input in) throws ISOException, UserException;


    /**
     * Entry point for merging static fields of Synchronizable instances.
     * @param in The source for the input.
     * @see #merge(Input)
     */
    protected void mergeStaticAll(Input in)
        throws ISOException, UserException {
        return;
    }


    /**
     * Iterate over all reacheable Synchronizable instances to merge them.
     * @param in The source for the input.
     * @param root The starting point for the iterative process.
     * @return Return starting point which might have changed.
     * @throws ISOException An ISOException can be thrown by {@link #addDuplicatedInstance(short, Synchronizable)}, {@link #getDuplicatedInstance(short)}, {@link Input#readN(short)}, {@link Input#getReadNValue(short)} or {@link #merge(Input)}.
     * @throws UserException An UserException can be thrown by {@link Input#readN(short)}, {@link Input#getReadNValue(short)} or {@link #merge(Input)}, if a new instance was created or if an instance is skipped.
     * @see Input#NEW_INSTANCE
     * @see Input#SKIP_INSTANCE
     */
    protected final static Synchronizable mergeAll(Input in, Synchronizable root)
        throws ISOException, UserException {

        if(currentInstance == null) {
            if(in.readN((short)5)) {
                short classId = (short)0;
                short instanceId = (short)0;
                switch(in.getReadNValue((short)0)) {
                    case Output.SKIP_INSTANCE:
                        UserException.throwIt(Input.SKIP_INSTANCE);
                    case Output.IS_DUPLICATE_REFERENCE:  // Since it is the first instance, it is coming from a static field; we return the instance
                        return in.currentApplet.getDuplicatedInstance((short)((in.getReadNValue((short)3) << 8) | in.getReadNValue((short)4)));

                    case Output.IS_DUPLICATED_INSTANCE:
                        classId = (short)((in.getReadNValue((short)1) << 8) | in.getReadNValue((short)2));
                        instanceId = (short)((in.getReadNValue((short)3) << 8) | in.getReadNValue((short)4));

                        if(root == null) {
                            in.newlyCreatedInstance = in.currentApplet.getNewInstance(in, classId);
                            in.newlyCreatedInstance.setId(instanceId);

                            in.currentApplet.addDuplicatedInstance(instanceId, in.newlyCreatedInstance);

                            UserException.throwIt(Input.NEW_INSTANCE);
                        } else if(in.currentApplet.getClassId(root) != classId) {
                            in.newlyCreatedInstance = in.currentApplet.getNewInstance(in, classId);
                            in.newlyCreatedInstance.setId(instanceId);

                            if(JCSystem.isObjectDeletionSupported())
                                JCSystem.requestObjectDeletion();

                            in.currentApplet.addDuplicatedInstance(instanceId, in.newlyCreatedInstance);

                            UserException.throwIt(Input.NEW_INSTANCE);
                        }

                        root.setId(instanceId);
                        in.currentApplet.addDuplicatedInstance(instanceId, root);
                        break;

                    case Output.IS_UNIQUE_REFERENCE:
                        classId = (short)((in.getReadNValue((short)1) << 8) | in.getReadNValue((short)2));
                        instanceId = (short)((in.getReadNValue((short)3) << 8) | in.getReadNValue((short)4)); 
                        if(root == null) {
                            in.newlyCreatedInstance = in.currentApplet.getNewInstance(in, classId);
                            in.newlyCreatedInstance.setId(instanceId);
                            UserException.throwIt(Input.NEW_INSTANCE);
                        } else if(in.currentApplet.getClassId(root) != classId) {
                            in.newlyCreatedInstance = in.currentApplet.getNewInstance(in, classId);
                            in.newlyCreatedInstance.setId(instanceId);
                            if(JCSystem.isObjectDeletionSupported())
                                JCSystem.requestObjectDeletion();
                            UserException.throwIt(Input.NEW_INSTANCE);
                        }
                        root.setId(instanceId);
                        break;

                    case Output.IS_NULL_REFERENCE:
                        if(root != null)
                            if(JCSystem.isObjectDeletionSupported())
                                JCSystem.requestObjectDeletion();
                        return null;
                }
            }

            if(root instanceof SynchronizableArrayFields) {
                ((SynchronizableArrayFields)root).setNext(instancesLinkedList);
                instancesLinkedList = (SynchronizableArrayFields)root;
            }

            root.setPreviousSynchronizableInstance(null);
            currentInstance = root;
            shouldSerializeOrMerge = true;
            in.resetResume();
        }

        do {
            if(shouldSerializeOrMerge) {
                currentInstance.merge(in);
                shouldSerializeOrMerge = false;
                in.resetResume();
            }

            if(currentInstance instanceof SynchronizableInstanceFields) {
                SynchronizableInstanceFields currentFather = (SynchronizableInstanceFields)currentInstance;

                if(currentFather.hasCurrentSynchronizableInstance()) {
                    in.readN((short)5);
                    in.resetResume();

                    if(in.getReadNValue((short)0) == Output.SKIP_INSTANCE) {
                        currentFather.selectNextSynchronizableInstance();
                    } else if(in.getReadNValue((short)0) == Output.IS_DUPLICATE_REFERENCE) {
                        currentFather.setCurrentSynchronizableInstance(in.currentApplet.getDuplicatedInstance((short)((in.getReadNValue((short)3) << 8) | in.getReadNValue((short)4))));
                        currentFather.selectNextSynchronizableInstance();
                    } else if(in.getReadNValue((short)0) == Output.IS_NULL_REFERENCE) {

                        if(currentFather.getCurrentSynchronizableInstance() != null) {
                            currentFather.setCurrentSynchronizableInstance(null);
                            if(JCSystem.isObjectDeletionSupported())
                                JCSystem.requestObjectDeletion();
                        }

                        currentFather.selectNextSynchronizableInstance();
                    } else {
                        short classId = (short)((in.getReadNValue((short)1) << 8) | in.getReadNValue((short)2));
                        short instanceId = (short)((in.getReadNValue((short)3) << 8) | in.getReadNValue((short)4));

                        Synchronizable nextInstance = currentFather.getCurrentSynchronizableInstance();

                        if(nextInstance == null) {
                            nextInstance = in.currentApplet.getNewInstance(in, classId);
                            currentFather.setCurrentSynchronizableInstance(nextInstance);
                        } else if(in.currentApplet.getClassId(nextInstance) != classId) {
                            nextInstance = in.currentApplet.getNewInstance(in, classId);
                            currentFather.setCurrentSynchronizableInstance(nextInstance);
                            if(JCSystem.isObjectDeletionSupported())
                                JCSystem.requestObjectDeletion();
                        }

                        currentFather.selectNextSynchronizableInstance();
                        nextInstance.setId(instanceId);

                        if(in.getReadNValue((short)0) == Output.IS_DUPLICATED_INSTANCE)
                            in.currentApplet.addDuplicatedInstance(instanceId, nextInstance);

                        if(nextInstance instanceof SynchronizableArrayFields) {
                            ((SynchronizableArrayFields)nextInstance).setNext(instancesLinkedList);
                            instancesLinkedList = (SynchronizableArrayFields)nextInstance;
                        }

                        nextInstance.setPreviousSynchronizableInstance(currentInstance);

                        currentInstance = nextInstance;
                        shouldSerializeOrMerge = true;
                    }
                } else {
                    currentFather.resetNextSynchronizableInstanceSelect();
                    currentInstance = currentInstance.getPreviousSynchronizableInstance();
                }
            } else {
                currentInstance = currentInstance.getPreviousSynchronizableInstance();
            }
        } while(currentInstance != null);

        return root;

    }


    /**
     * This method is call by the SyncManager to start the merge process.
     * @param shouldResume If true, the merge process is resumed else it's started.
     * @throws ISOException An ISOException is thrown if the caller is not the SyncManager.
     * @throws UserException An UserException is thrown if the read process is interrupted.
     */
    public final void syncMerge(boolean shouldResume)
        throws ISOException, UserException {

        if(!JCSystem.getPreviousContextAID().equals(syncAID))
            ISOException.throwIt(SyncSW.IS_NOT_sync);

        if(shouldResume) {
            in.resume();
        } else {
            SyncInput syncIn = (SyncInput)JCSystem.getAppletShareableInterfaceObject(syncAID, (byte)2);

            JCSystem.beginTransaction();
            isReady = false;
            JCSystem.commitTransaction();

            in.start(syncIn);

            currentInstance = null;
            isStaticDone = false;
            nbDuplicatedInstances = (short)0;

            this.resetStaticPathAll(in);
            resetPathAll(in, this);
        }

        if(!isStaticDone) {
            this.mergeStaticAll(in);
            in.resetResume();
            currentInstance = null;
            isStaticDone = true;
        }

        mergeAll(in, this);

        in.done();

        JCSystem.beginTransaction();
        isReady = true;
        JCSystem.commitTransaction();

    }


    public Shareable getShareableInterfaceObject(AID clientAID, byte parameter) {

        if(clientAID.equals(syncAID))
            return this;

        return null;

    }


    /**
     * Unregister the child instance from sync when an uninstall event occurs.
     */ 
    protected final void syncUninstall() {

        SyncService aSyncService = (SyncService)JCSystem.getAppletShareableInterfaceObject(syncAID, (byte)0);
        aSyncService.unregister();

        currentInstance = null;

        short i = (short)0;
        while((i < maxNbDuplicatedInstances) && (duplicatedInstances[i] != null))
            duplicatedInstances[i++].instance = null;

        instancesLinkedList = null;

    }

    public void uninstall() {

        syncUninstall();

    }

}
