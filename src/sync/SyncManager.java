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

package sync;

import synclib.SyncSynchronizable;
import synclib.SyncService;
import synclib.SyncOutput;
import synclib.SyncInput;
import synclib.SyncINS;
import synclib.SyncSW;

import javacard.framework.AID;
import javacard.framework.APDU;
import javacard.framework.Applet;
import javacard.framework.ISOException;
import javacard.framework.JCSystem;
import javacard.framework.Shareable;
import javacard.framework.ISO7816;
import javacard.framework.Util;
import javacard.framework.UserException;
import javacard.security.AESKey;
import javacard.security.Key;
import javacard.security.KeyBuilder;
import javacard.security.RandomData;

/**
 * This class takes care of the incoming apdu for sync like a serialization command or a merge command.
 */
class SyncManager extends Applet {

    /**
     * The sync AES key (should not be here but specified at install.)
     */
    private static final byte[] rawSyncKey = {(byte)0x49, (byte)0x8C, (byte)0xEB, (byte)0xA5, (byte)0x43, (byte)0x2A, (byte)0x0C, (byte)0x08, (byte)0x80, (byte)0x53, (byte)0x25, (byte)0xF4, (byte)0x94, (byte)0xE0, (byte)0x62, (byte)0x75};

    /**
     * The sync AES initial vector should not be here but specified at install.)
     */
    private static final byte[] syncIV = {(byte)0xF4, (byte)0x21, (byte)0x2C, (byte)0xFD, (byte)0x3A, (byte)0x86, (byte)0x2C, (byte)0x1E, (byte)0x59, (byte)0x2F, (byte)0x16, (byte)0x16, (byte)0x14, (byte)0x1F, (byte)0x49, (byte)0xE6};

    /**
     * The user AES key.
     */
    private byte[] rawUserKey = null;

    /**
     * The user default AES initial vector.
     */
    private byte[] defaultUserIV = null;

    /**
     * The user transient AES initial vector. The byte at index 16 is used as a state for this array.
     * 0: Nothing in progress
     * 1: Serialization waiting for merge IV part
     * 2: Merge in progress
     * 3: Serialization in progress
     */
    private byte[] transientUserIV = null;

    /**
     * The list of AIDs managed by sync.
     */
    private AIDList anAIDList = null;

    /**
     * A reference to a shareable interface for the registration of applet whitin sync.
     */
    private SyncService aSyncService = null;

    /**
     * A reference to an output used for the serialization of an applet.
     */
    private SyncOutput output = null;

    /**
     * A reference to an input used for the merge of an applet.
     */
    private SyncInput input = null;

    /**
     * Transient applet's id array to notify an ongoing serialization or merge. Serialization is at index 0, merge at index 1.
     */
    private short[] ongoingIds = null;

    /**
     * A reference to the cipher used to encrypt/decrypt APDU.
     */
    private CryptoAPDU cipher = null;

    /**
     * An instance of a random generator used for the challenge generation and the cipher padding.
     */
    private RandomData generator = null;

    /**
     * The userkey instance.
     */
    private AESKey userKey = null;

    /**
     * The sync key instance.
     */
    private AESKey syncKey = null;

    /**
     * Indicate if this sync device is configured or not.
     */
    private boolean isConfigured = false;

    /**
     * The user's fingerprint data (for demo purpose.)
     */
    private static final byte[] userFingerprint = {(byte)0xDE, (byte)0xAD, (byte)0xBE, (byte)0xEF};

    /**
     * The state machine of the challenge process (for demo purpose.)
     */
    private byte[] crtChallenge = null;


    /**
     * Create an SyncManager with output and input using the apdu implementation.
     */ 
    private SyncManager() {

        anAIDList = new AIDList();
        aSyncService = new SyncServiceWrapper(anAIDList);

        syncKey = (AESKey)KeyBuilder.buildKey(KeyBuilder.TYPE_AES, KeyBuilder.LENGTH_AES_128, false);
        syncKey.setKey(rawSyncKey, (short)0);

        rawUserKey = new byte[16];
        defaultUserIV = new byte[17];
        transientUserIV = JCSystem.makeTransientByteArray((short)17, JCSystem.CLEAR_ON_RESET);
        userKey = (AESKey)KeyBuilder.buildKey(KeyBuilder.TYPE_AES, KeyBuilder.LENGTH_AES_128, false);

        generator = RandomData.getInstance(RandomData.ALG_PSEUDO_RANDOM);
        cipher = new AES128CryptoAPDU(generator);
        output = new SyncOutputCryptoAPDU(cipher);
        input = new SyncInputCryptoAPDU(cipher);
        ongoingIds = JCSystem.makeTransientShortArray((short)2, JCSystem.CLEAR_ON_DESELECT);

        /* for demo purpose */
        crtChallenge = JCSystem.makeTransientByteArray((short)33, JCSystem.CLEAR_ON_DESELECT);

    }


    /**
     * Return a shareable interface depending on the byte parameter.
     * Only registered applets can ask for the output or input shareable interface.
     * @param parameter if 0, the SyncService interface is returned. If 1, the output interface is returned. If 2, the input interface is returned.
     * @return The requested shareable interface or null if the applet is not authorized to make the request.
     */
    public Shareable getShareableInterfaceObject(AID clientAID, byte parameter) {

        if(parameter == (byte)0)
            return aSyncService;

        if(anAIDList.indexOf(clientAID) == (short)-1)
            return null;

        switch(parameter) {
            case (byte)1:
                return output;
            case (byte)2:
                return input;
        }

        return null;

    }


    /**
     * Increment the IV for ciphering in between serialization/merge APDUs.
     */
    private void incrementTransientUserIV() {

        short i = (short)0;
        do {

            short newValue = (short)((transientUserIV[i] & 0x00FF) + (short)1);
            if(newValue == (short)0x00FF) {
                transientUserIV[i] = (byte)0;
                i++;
            } else {
                transientUserIV[i] = (byte)newValue;
                return;
            }

        } while(i < (short)(transientUserIV.length - 1));

    }


    /**
     * Create an instance of the SyncManager and register it.
     */
    public static void install(byte bArray[], short bOffset, byte bLength)
            throws ISOException {

        new SyncManager().register();

    }


    /**
     * Process an apdu.
     * @param apdu The apdu to process.
     * @throws ISOException An ISOException can be thrown.
     * @see synclib.SyncINS
     * @see synclib.SyncSW
     */
    public void process(APDU apdu)
        throws ISOException {

        byte[] buffer = apdu.getBuffer();

        if(buffer[ISO7816.OFFSET_CLA] == (byte)SyncINS.CLA) {
            AID appletAID = null;
            short requestedId = 0;
            SyncSynchronizable aSynchronizableApplet = null;

            boolean shouldResume = false;   /* Should the current (if any) serialization/merge process be resumed? */
            boolean shouldReplay = false;   /* Should the current (if any) serialization/merge process be replayed? */
            short ongoingSerializationId = ongoingIds[0];
            short ongoingMergeId = ongoingIds[1];

            short lc = (short)buffer[ISO7816.OFFSET_LC];

            ongoingIds[0] = 0;
            ongoingIds[1] = 0;

            switch(buffer[ISO7816.OFFSET_INS]) {
                case ISO7816.INS_SELECT:
                    break;

                case SyncINS.GET_AID_BY_ID:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);

                    anAIDList.getAnAID();
                    break;

                case SyncINS.GET_NB_AIDS:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);

                    anAIDList.nbAIDs();
                    break;

                case SyncINS.START_SERIALIZE_SESSION:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);

                    if(lc == (short)0) {    /* If there is no payload then a new serialization session is starting. */
                        /* We generate a random half IV for ciphering the serialization stream. */
                        generator.generateData(transientUserIV, (short)0, (short)8);
                        Util.arrayCopyNonAtomic(transientUserIV, (short)0, buffer, (short)0, (short)8);

                        /* We cipher the half IV for sending to the merging device using user ciphering. */
                        cipher.init(userKey, defaultUserIV);
                        lc = cipher.encryptAPDU((short)0, (short)8);

                        apdu.setOutgoingAndSend((short)0, lc);
                        transientUserIV[16] = (byte)1;
                    } else if(lc == (short)16 && transientUserIV[16] == (byte)1) {  /* We got the other random half IV from the merging device. */
                        cipher.init(userKey, defaultUserIV);
                        if(!cipher.decryptAPDU((short)ISO7816.OFFSET_CDATA, (short)16)) {
                            transientUserIV[16] = (byte)0;
                            ISOException.throwIt(ISO7816.SW_WRONG_DATA);
                        }

                        /* We check if the serialization random half IV is the right one. */
                        if(Util.arrayCompare(transientUserIV, (short)0, buffer, (short)ISO7816.OFFSET_CDATA, (short)8) != (byte)0) {
                            transientUserIV[16] = (byte)0;
                            ISOException.throwIt(ISO7816.SW_WRONG_DATA);
                        }

                        /* We fetch the merging random half IV. */
                        Util.arrayCopyNonAtomic(buffer, (short)(ISO7816.OFFSET_CDATA + 8), transientUserIV, (short)8, (short)8);
                        transientUserIV[16] = (byte)3;
                    } else {
                        transientUserIV[16] = (byte)0;
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);
                    }
                    break;

                case SyncINS.SERIALIZE_APPLET_BY_ID:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);
                    if(ongoingMergeId != (short)0)
                        ISOException.throwIt(SyncSW.MERGE_INS_EXPECTED);
                    if(transientUserIV[16] != (byte)3)
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    /* We check for requests comming from the merging device. */
                    cipher.init(userKey, transientUserIV);
                    if(lc != (short)0) {
                        if(!cipher.decryptAPDU((short)ISO7816.OFFSET_CDATA, lc))
                            ISOException.throwIt(ISO7816.SW_WRONG_DATA);

                        if(buffer[ISO7816.OFFSET_CDATA] == SyncInput.WAITING_FOR_MORE)
                            shouldResume = true;
                        else if(buffer[ISO7816.OFFSET_CDATA] == SyncInput.REPLAY_THIS_APDU)
                            shouldReplay = true;
                        else 
                            ISOException.throwIt(ISO7816.SW_INS_NOT_SUPPORTED);
                    }

                    if(shouldResume && (ongoingSerializationId == (short)0))
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    appletAID = anAIDList.getAIDById();
                    requestedId = (short)(buffer[ISO7816.OFFSET_P1] & 0x00FF);

                    if(shouldResume && (ongoingSerializationId != requestedId))
                        ISOException.throwIt(SyncSW.WRONG_ID_RESUME);

                    aSynchronizableApplet = (SyncSynchronizable)JCSystem.getAppletShareableInterfaceObject(appletAID, (byte)0);

                    try {
                        aSynchronizableApplet.syncSerialize(shouldResume, shouldReplay);
                    }
                    catch(UserException e) {            /* If there is an UserException then it should be the size of the outgoing APDU.*/
                        ongoingIds[0] = requestedId;    /* Since there was an UserException, we save the id of the serialized applet since it is not done.*/
                        apdu.setOutgoingAndSend((short)0, e.getReason());
                    }

                    incrementTransientUserIV();
                    break;

                case SyncINS.START_MERGE_SESSION:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);

                    if(lc == (short)0)  /* There should be payload with the serialization random half IV. */
                        ISOException.throwIt(ISO7816.SW_WRONG_DATA);

                    cipher.init(userKey, defaultUserIV);
                    if(!cipher.decryptAPDU((short)ISO7816.OFFSET_CDATA, lc)) {
                        transientUserIV[16] = 0;
                        ISOException.throwIt(ISO7816.SW_WRONG_DATA);
                    }

                    /* We fetch the serialization random half IV and generate the other half.*/
                    Util.arrayCopyNonAtomic(buffer, (short)ISO7816.OFFSET_CDATA, transientUserIV, (short)0, (short)8);
                    generator.generateData(transientUserIV, (short)8, (short)8);
                    Util.arrayCopyNonAtomic(transientUserIV, (short)0, buffer, (short)0, (short)16); 

                    cipher.init(userKey, defaultUserIV);
                    lc = cipher.encryptAPDU((short)0, (short)16);

                    apdu.setOutgoingAndSend((short)0, lc);
                    transientUserIV[16] = (byte)2;
                    break;

                case SyncINS.MERGE_APPLET_BY_ID:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);
                    if(ongoingSerializationId != (short)0)
                        ISOException.throwIt(SyncSW.SERIALIZE_INS_EXPECTED);
                    if(transientUserIV[16] != 2)
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    if(ongoingMergeId != 0)     /* A previous merge process was not done. */
                        shouldResume = true;

                    appletAID = anAIDList.getAIDById();
                    requestedId = (short)(buffer[ISO7816.OFFSET_P1] & 0x00FF);

                    if(shouldResume && (ongoingMergeId != requestedId))
                        ISOException.throwIt(SyncSW.WRONG_ID_RESUME);

                    aSynchronizableApplet = (SyncSynchronizable)JCSystem.getAppletShareableInterfaceObject(appletAID, (byte)0);

                    cipher.init(userKey, transientUserIV);

                    try {
                        aSynchronizableApplet.syncMerge(shouldResume);
                    }
                    catch(UserException e) {        /* If there is an UserException then there is a merge request to send to the serializing device. */
                        ongoingIds[1] = requestedId;    /* There was an UserException thus the merge process is not done. */
                        incrementTransientUserIV();
                        cipher.init(userKey, transientUserIV);
                        buffer[0] = (byte)(e.getReason() & 0x00FF);     /* Here come the request */
                        lc = cipher.encryptAPDU((short)0, (short)1);
                        apdu.setOutgoingAndSend((short)0, lc);
                        break;
                    }

                    incrementTransientUserIV();
                    ISOException.throwIt(SyncSW.MERGE_DONE);
                    break;

                case SyncINS.END_SESSION:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);

                    transientUserIV[16] = 0;
                    break;

                case SyncINS.LIST_AIDS:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);

                    anAIDList.listAIDs();
                    break;

                case SyncINS.CHECK_INSTALL:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);

                    AID AIDToCheck = JCSystem.lookupAID(buffer, (short)ISO7816.OFFSET_CDATA, (byte)(lc & 0xFF));
                    if(AIDToCheck == null)
                        ISOException.throwIt(SyncSW.APPLET_NOT_FOUND);

                    requestedId = anAIDList.indexOf(AIDToCheck);

                    if(requestedId == (short)-1)
                        ISOException.throwIt(SyncSW.AID_NOT_FOUND);

                    ISOException.throwIt(Util.makeShort((byte)0x90,(byte)(requestedId + (short)1)));

                    break;

/********************************************************************
 * For demo purpose
 */
                case SyncINS.START_CONFIG:
                    if(isConfigured || (crtChallenge[0] != (byte)0x00))
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    if((buffer[ISO7816.OFFSET_P1] != (byte)0) || (buffer[ISO7816.OFFSET_P2] != (byte)0))
                        ISOException.throwIt(ISO7816.SW_WRONG_P1P2);

                    if(lc != (short)0)
                        ISOException.throwIt(ISO7816.SW_WRONG_LENGTH);

                    crtChallenge[0] = SyncINS.START_CONFIG;

                    break;

                case SyncINS.GET_SERVER_CHALLENGE:
                    if(!isConfigured || (crtChallenge[0] != (byte)0))
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    generator.generateData(crtChallenge, (short)1, (short)(crtChallenge.length - 1));
                    Util.arrayCopyNonAtomic(crtChallenge, (short)1, buffer, (short)0, (short)(crtChallenge.length - 1));

                    cipher.init(syncKey, syncIV);
                    lc = cipher.encryptAPDU((short)0, (short)(crtChallenge.length - 1));

                    apdu.setOutgoingAndSend((short)0, lc);
                    crtChallenge[0] = SyncINS.GET_SERVER_CHALLENGE;

                    break;

                case SyncINS.ANSWER_SERVER_CHALLENGE:
                    if(isConfigured || (crtChallenge[0] != SyncINS.START_CONFIG))
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    cipher.init(syncKey, syncIV);
                    cipher.decryptAPDU((short)ISO7816.OFFSET_CDATA, lc);

                    Util.arrayCopyNonAtomic(buffer, (short)ISO7816.OFFSET_CDATA, buffer, (short)0, lc);

                    generator.generateData(crtChallenge, (short)1, (short)(crtChallenge.length -1));
                    Util.arrayCopyNonAtomic(crtChallenge, (short)1, buffer, lc, (short)(crtChallenge.length - 1));
                    lc += (short)(crtChallenge.length + crtChallenge.length - 2);

                    lc = cipher.encryptAPDU((short)0, lc);
                    apdu.setOutgoingAndSend((short)0, lc);

                    crtChallenge[0] = SyncINS.ANSWER_SERVER_CHALLENGE;

                    break;

                case SyncINS.ANSWER_CLIENT_CHALLENGE:
                    if(!isConfigured || (crtChallenge[0] != SyncINS.GET_SERVER_CHALLENGE))
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    cipher.init(syncKey, syncIV);
                    cipher.decryptAPDU((short)ISO7816.OFFSET_CDATA, lc);

                    if(Util.arrayCompare(buffer, (short)ISO7816.OFFSET_CDATA, crtChallenge, (short)1, (short)(crtChallenge.length - 1)) != 0) {
                        crtChallenge[0] = (byte)0x00;
                        ISOException.throwIt((short)0x4242);
                    }

                    Util.arrayCopyNonAtomic(buffer, (short)(ISO7816.OFFSET_CDATA + crtChallenge.length - 1), buffer, (short)0, (short)(crtChallenge.length - 1));
                    Util.arrayCopyNonAtomic(crtChallenge, (short)1, buffer, (short)(crtChallenge.length - 1), (short)(crtChallenge.length - 1));

                    lc = cipher.encryptAPDU((short)0, lc);
                    apdu.setOutgoingAndSend((short)0, lc);

                    crtChallenge[0] = SyncINS.ANSWER_CLIENT_CHALLENGE;

                    break;

                case SyncINS.GET_FINGERPRINT:
                    if(isConfigured || (crtChallenge[0] != SyncINS.ANSWER_SERVER_CHALLENGE))
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    cipher.init(syncKey, syncIV);
                    cipher.decryptAPDU((short)ISO7816.OFFSET_CDATA, lc);

                    if(Util.arrayCompare(buffer, (short)ISO7816.OFFSET_CDATA, crtChallenge, (short)1, (short)(crtChallenge.length - 1)) != 0) {
                        crtChallenge[0] = (byte)0x00;
                        ISOException.throwIt(SyncSW.CHALLENGE_FAILED);
                    }

                    Util.arrayCopyNonAtomic(buffer, (short)(ISO7816.OFFSET_CDATA + crtChallenge.length - 1), buffer, (short)0, (short)(crtChallenge.length - 1));
                    Util.arrayCopyNonAtomic(crtChallenge, (short)1, buffer, (short)(crtChallenge.length - 1), (short)(crtChallenge.length - 1));
                    Util.arrayCopyNonAtomic(userFingerprint, (short)0, buffer, (short)(crtChallenge.length + crtChallenge.length - 2), (short)userFingerprint.length);
                    lc += (short)userFingerprint.length;

                    lc = cipher.encryptAPDU((short)0, lc);
                    apdu.setOutgoingAndSend((short)0, lc);

                    crtChallenge[0] = SyncINS.GET_FINGERPRINT;

                    break;

                case SyncINS.GET_USERKEY:
                    if(!isConfigured || (crtChallenge[0] != SyncINS.ANSWER_CLIENT_CHALLENGE))
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    cipher.init(syncKey, syncIV);
                    cipher.decryptAPDU((short)ISO7816.OFFSET_CDATA, lc);

                    if(Util.arrayCompare(buffer, (short)ISO7816.OFFSET_CDATA, crtChallenge, (short)1, (short)(crtChallenge.length - 1)) != 0) {
                        crtChallenge[0] = (byte)0x00;
                        ISOException.throwIt(SyncSW.CHALLENGE_FAILED);
                    }

                    lc = (short)(crtChallenge.length - 1);
                    Util.arrayCopyNonAtomic(buffer, (short)(ISO7816.OFFSET_CDATA + lc), buffer, (short)0, lc);
                    Util.arrayCopyNonAtomic(crtChallenge, (short)1, buffer, lc, lc);
                    lc += lc;
                    Util.arrayCopyNonAtomic(rawUserKey, (short)0, buffer, lc, (short)rawUserKey.length);
                    lc += rawUserKey.length;
                    Util.arrayCopyNonAtomic(defaultUserIV, (short)0, buffer, lc, (short)defaultUserIV.length);
                    lc += defaultUserIV.length;

                    lc = cipher.encryptAPDU((short)0, lc);
                    apdu.setOutgoingAndSend((short)0, lc);

                    crtChallenge[0] = (byte)0x00;

                    break;

                case SyncINS.INSTALL_USERKEY:
                    if(isConfigured || (crtChallenge[0] != SyncINS.GET_FINGERPRINT))
                        ISOException.throwIt(SyncSW.INS_NOT_EXPECTED);

                    cipher.init(syncKey, syncIV);
                    cipher.decryptAPDU((short)ISO7816.OFFSET_CDATA, lc);

                    if(Util.arrayCompare(buffer, (short)ISO7816.OFFSET_CDATA, crtChallenge, (short)1, (short)(crtChallenge.length - 1)) != 0) {
                        crtChallenge[0] = (byte)0x00;
                        ISOException.throwIt(SyncSW.CHALLENGE_FAILED);
                    }

                    JCSystem.beginTransaction();
                        Util.arrayCopy(buffer, (short)(ISO7816.OFFSET_CDATA + crtChallenge.length + crtChallenge.length - 2), rawUserKey, (short)0, (short)rawUserKey.length);
                        Util.arrayCopy(buffer, (short)(ISO7816.OFFSET_CDATA + crtChallenge.length + crtChallenge.length - 2 + rawUserKey.length), defaultUserIV, (short)0, (short)defaultUserIV.length);
                        userKey.setKey(rawUserKey, (short)0);
                        isConfigured = true;
                    JCSystem.commitTransaction();

                    crtChallenge[0] = (byte)0x00;

                    break;

                case SyncINS.ABORT_CHALLENGE:
                    crtChallenge[0] = (byte)0x00;

                    break;

                case SyncINS.IS_CONFIGURED:
                    if(!isConfigured)
                        ISOException.throwIt(SyncSW.NOT_CONFIGURED);

                    break;

/******************************************************************
 * TODO: TO REMOVE LATER
 */
                case SyncINS.DUMMY_INSTALL_USERKEY:
                    Util.arrayCopy(buffer, (short)ISO7816.OFFSET_CDATA, rawUserKey, (short)0, (short)rawUserKey.length);
                    Util.arrayCopy(buffer, (short)(ISO7816.OFFSET_CDATA + rawUserKey.length), defaultUserIV, (short)0, (short)defaultUserIV.length);
                    userKey.setKey(rawUserKey, (short)0);
                    isConfigured = true;

                    break;

                case SyncINS.DUMMY_SET_SEED:
                    generator.setSeed(buffer, ISO7816.OFFSET_CDATA, buffer[ISO7816.OFFSET_LC]);

                    break;

/******************************************************************
 */
                default:
                    ISOException.throwIt(ISO7816.SW_INS_NOT_SUPPORTED);
            }
        } else
            if(buffer[ISO7816.OFFSET_INS] != ISO7816.INS_SELECT)
                ISOException.throwIt(ISO7816.SW_CLA_NOT_SUPPORTED);
    }

}
