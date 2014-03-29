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

package test_applet_1;

import synclib.Input;
import synclib.Output;
import synclib.ResetIdSource;
import synclib.Synchronizable;
import synclib.SynchronizableArrayFields;

import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.UserException;

class SomeTestClass1 implements Synchronizable, SynchronizableArrayFields {

    private final static short sArraySize = (short)16;
    short[] sArray;

    protected short id;
    protected boolean isDuplicated = false;
    protected Synchronizable previous = null;
    protected SynchronizableArrayFields next = null;
    private short currentField = (short)0;

    SomeTestClass1() {
        sArray = new short[sArraySize];
        for(short i = (short)0; i < sArraySize; i++)
            sArray[i] = i;
    }

    public SomeTestClass1(Input in) {
        if(in == null)
            ISOException.throwIt(ISO7816.SW_UNKNOWN);
    }

    public void setPreviousSynchronizableInstance(Synchronizable previous) {
        this.previous = previous;
    }

    public Synchronizable getPreviousSynchronizableInstance() {
        return previous;
    }

    public boolean resetPath(ResetIdSource source) {

        short resetId = source.getResetId();
        if(id == resetId)
            return false;

        id = resetId;
        isDuplicated = false;
        currentField = (short)0;

        return true;

    }

    public boolean numberPath(Output out) {

        if(id > (short)-1) {
            isDuplicated = true;
            return false;
        }

        id = out.getNextId();

        return true;

    }

    public short getId() {
        return id;
    }

    public void setId(short id) {
        this.id = id;
    }

    public void setNext(SynchronizableArrayFields next) {
        this.next = next;
    }

    public SynchronizableArrayFields getNext() {
        return next;
    }

    public boolean isDuplicated() {
        return isDuplicated;
    }

    public short getNbBooleanArrays() {
        return 0;
    }

    public boolean[] getBooleanArray(short fieldId) {
        ISOException.throwIt(ISO7816.SW_UNKNOWN);
        return null;
    }

    public short getNbByteArrays() {
        return 0;
    }

    public byte[] getByteArray(short fieldId) {
        ISOException.throwIt(ISO7816.SW_UNKNOWN);
        return null;
    }

    public short getNbShortArrays() {
        return 1;
    }

    public short[] getShortArray(short fieldId) {
        switch(fieldId) {
            case (short)0:
                return sArray;
        }
        ISOException.throwIt(ISO7816.SW_UNKNOWN);
        return null;
    }

    public void serialize(Output out)
        throws ISOException, UserException {

        out.write(sArray, (short)0);

    }

    public void merge(Input in)
        throws ISOException, UserException {

        switch(currentField) {
            case (short)0:
                sArray = in.checkArray(sArray);
                in.resetResume();
                ++currentField;

            case (short)1:
                in.read(sArray);
        }
        currentField = (short)0;

    }

}
