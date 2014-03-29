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

import synclib.SyncSW;
import javacard.framework.AID;
import javacard.framework.APDU;
import javacard.framework.APDUException;
import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.JCSystem;

/**
 * This class represents a list of AIDs managed by sync.
 * The AIDs are references to AID instances owned by the VM.
 * This class should be only used within SyncManager.
 */
class AIDList {

    /**
     * The maximum number of AIDs the list can contain.
     */
    private static final short MAX_NB_AIDS = (short)255;

    /**
     * The number of AIDs currently in the list.
     */
    private short nbAIDs = (short)0;

    /**
     * The array containing the AIDs. Unoccupied cells contain null.
     */
    private AID[] list;


    /**
     * Create a list of AIDs.
     */
    AIDList() {

        list = new AID[MAX_NB_AIDS];

    }


    /**
     * List the AIDs present in the list through the apdu buffer.
     * A call to sendBytes is used for each AID.
     * @throws ISOException An ISOException is thrown if an APDUException occurs.
     */
    void listAIDs()
        throws ISOException {

        APDU apdu = APDU.getCurrentAPDU();
        byte[] buffer = apdu.getBuffer();
        short totalLength = (short)0;

        try { 
            apdu.setOutgoing();
        } catch(APDUException e) {
            ISOException.throwIt(SyncSW.APDU_EXCEPTION);
        }

        for(short i = (short)0; i < nbAIDs; i++)
            totalLength += list[i].getBytes(buffer, (short)0) + 1;

        try {
            apdu.setOutgoingLength(totalLength);
        } catch(APDUException e) {
            ISOException.throwIt(SyncSW.APDU_EXCEPTION);
        }

        for(short i = (short)0; i < nbAIDs; i++) {
                buffer[0] = list[i].getBytes(buffer, (short)1);

            try {
                apdu.sendBytes((short)0, (short)(buffer[0] + 1));
            } catch(APDUException e) {
                ISOException.throwIt(SyncSW.APDU_EXCEPTION);
            }
        }

    }


    /**
     * Respond to an apdu command for getting an AID using an id.
     * A call to setOutgoingAndSend is used to give the requested AID.
     * @throws ISOException An ISOException is thrown if an APDUException occurs, if a wrong id is requested or if the supplied number of stocked AIDs is wrong.
     */
    void getAnAID()
        throws ISOException {

        AID AIDToSend = getAIDById();
        APDU apdu = APDU.getCurrentAPDU();
        byte[] buffer = apdu.getBuffer();

        try {
            apdu.setOutgoingAndSend((short)0, (short)AIDToSend.getBytes(buffer, (short)0));
        } catch(APDUException e) {
            ISOException.throwIt(SyncSW.APDU_EXCEPTION);
        }

    }


    /**
     * Respond to an apdu command for getting the current number of stocked AIDs.
     * A call to setOutgoingAndSend is used to give the current number of stocked AIDs.
     * @throws ISOException AN ISOException is thrown if an APDUException occurs.
     */
    void nbAIDs()
        throws ISOException {

        APDU apdu = APDU.getCurrentAPDU();
        byte[] buffer = apdu.getBuffer();
        buffer[0] = (byte)nbAIDs;

        try {
            apdu.setOutgoingAndSend((short)0, (short)1);
        } catch(APDUException e) {
            ISOException.throwIt(SyncSW.APDU_EXCEPTION);
        }

    }


    /**
     * Add an AID reference to the list in an atomicaly fashion.
     * If the list is full, the AID is not added.
     * @param AIDToAdd The AID to add in the list.
     */
    void addAID(AID AIDToAdd) {

        if(nbAIDs < MAX_NB_AIDS) {
            boolean hasTransactionBeginHere = false;

            if(JCSystem.getTransactionDepth() == 0) {
                JCSystem.beginTransaction();
                hasTransactionBeginHere = true;
            }

            list[nbAIDs] = AIDToAdd;
            nbAIDs++;

            if(hasTransactionBeginHere)
                JCSystem.commitTransaction();
        }

    }


    /**
     * Remove an AID from the list in an atomicaly fashion.
     * @param AIDToDelete The AID to delete. The equals method is used to find the AID to delete. The list is packed after deletion.
     */
    void deleteAID(AID AIDToDelete) {

        for(short i = (short)0; i < nbAIDs; i++)
            if(list[i].equals(AIDToDelete)) {
                boolean hasTransactionBeginHere = false;

                if(JCSystem.getTransactionDepth() == 0) {
                    JCSystem.beginTransaction();
                    hasTransactionBeginHere = true;
                }

                for(short j = (short)(i + 1); j < nbAIDs; j++)
                    list[(short)(j - 1)] = list[j];

                nbAIDs--;
                list[nbAIDs] = null;

                if(hasTransactionBeginHere)
                    JCSystem.commitTransaction();

                return;
            }

    }


    /**
     * Process the current apdu to return a reference to the resquest AID by id.
     * @throws ISOException An ISOException is thrown if a wrong id is requested or if the supplied number of stocked AIDs is wrong.
     * @return A reference to the requested AID.
     */
    AID getAIDById()
        throws ISOException {

        APDU apdu = APDU.getCurrentAPDU();
        byte[] buffer = apdu.getBuffer();
        short P1 = (short)(buffer[ISO7816.OFFSET_P1] & 0x00FF);
        short P2 = (short)(buffer[ISO7816.OFFSET_P2] & 0x00FF);
        short nbAIDsSinceStart = (short)0;

        if(P2 != nbAIDs)
            ISOException.throwIt(SyncSW.WRONG_NB_AIDS);

        if((P1 > nbAIDs) || (P1 < (short)1))
            ISOException.throwIt(SyncSW.WRONG_ID);

        return list[(short)(P1 - 1)];

    }


    /**
     * Search in the list for the first occurence of anAID and return its index.
     * The search is done using the equals method.
     * @param anAID The AID to search in the list.
     * @return Return the index of the AID is found, -1 else.
     */
    short indexOf(AID anAID) {

        for(short i = (short)0; i < nbAIDs; i++)
            if(list[i].equals(anAID))
                return i;

        return (short)-1;

    }

}
