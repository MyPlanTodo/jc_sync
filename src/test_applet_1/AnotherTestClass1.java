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
import synclib.SynchronizableInstanceFields;

import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.UserException;

class AnotherTestClass1 implements Synchronizable, SynchronizableArrayFields, SynchronizableInstanceFields {

    private final static short bArraySize = (short)16;
    byte[] bArray;
    SomeTestClass1 someTestClass1 = null;

    protected short id;
    protected boolean isDuplicated = false;
    protected Synchronizable previous = null;
    private short currentField = (short)0;
    protected SynchronizableArrayFields next = null;

    AnotherTestClass1() {
        bArray = new byte[bArraySize];
        for(short i = (short)0; i < bArraySize; i++)
            bArray[i] = (byte)i;
    }

    public AnotherTestClass1(Input in) {
        if(in == null)
            ISOException.throwIt(ISO7816.SW_UNKNOWN);
    }

    public void setPreviousSynchronizableInstance(Synchronizable previous) {
        this.previous = previous;
    }

    public boolean hasCurrentSynchronizableInstance() {
        return currentField < (short)1;
    }

    public void selectNextSynchronizableInstance() {

        if(currentField < (short)1)
            ++currentField;

    }

    public Synchronizable getCurrentSynchronizableInstance()
        throws ClassCastException {

        switch(currentField) {
            case (short)0:
                return (Synchronizable)someTestClass1;
        }
        return null;

    }

    public void setCurrentSynchronizableInstance(Synchronizable instance) {
        switch(currentField) {
            case (short)0:
                someTestClass1 = (SomeTestClass1)instance;
                return;
        }
    }

    public void resetNextSynchronizableInstanceSelect() {
        currentField = (short)0;
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
        return 1;
    }

    public byte[] getByteArray(short fieldId) {
        switch(fieldId) {
            case (short)0:
                return bArray;
        }
        ISOException.throwIt(ISO7816.SW_UNKNOWN);
        return null;
    }

    public short getNbShortArrays() {
        return 0;
    }

    public short[] getShortArray(short fieldId) {
        ISOException.throwIt(ISO7816.SW_UNKNOWN);
        return null;
    }

    public void serialize(Output out)
        throws ISOException, UserException {

        out.write(bArray, (short)0);

    }

    public void merge(Input in)
        throws ISOException, UserException {

        switch(currentField) {
            case (short)0:
                bArray = in.checkArray(bArray);
                in.resetResume();
                ++currentField;
            case (short)1:
                in.read(bArray);
        }
        currentField = (short)0;

    }

}
