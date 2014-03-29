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

import synclib.SyncApplet;
import synclib.Input;
import synclib.Output;
import synclib.ResetIdSource;
import synclib.Synchronizable;
import synclib.SynchronizableArrayFields;
import synclib.SynchronizableInstanceFields;

import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.UserException;
import javacard.framework.APDU;
import javacard.framework.Applet;

public class SuperTestApplet1 extends SyncApplet implements Synchronizable, SynchronizableArrayFields, SynchronizableInstanceFields {

    protected short someValue = (short)0x4242;
    protected AnotherTestClass1 anotherTestClass1 = null;
    protected short[] sArray = null;

    private short id;
    private boolean isDuplicated = false;
    private Synchronizable previous = null;
    private short currentField = (short)0;
    private SynchronizableArrayFields next = null;

    public SuperTestApplet1() {
        anotherTestClass1 = new AnotherTestClass1();
    }

    public SuperTestApplet1(Input in) {
        if(in == null)
            ISOException.throwIt(ISO7816.SW_UNKNOWN);
    }

    public void process(APDU apdu)
        throws ISOException {
        super.process(apdu);
        return;
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
                return (Synchronizable)anotherTestClass1;
        }
        return null;

    }

    public void setCurrentSynchronizableInstance(Synchronizable instance) {

        switch(currentField) {
            case (short)0:
                anotherTestClass1 = (AnotherTestClass1)instance;
                return;
        }

    }

    public void resetNextSynchronizableInstanceSelect() {
        currentField = (short)0;
    }

    public Synchronizable getPreviousSynchronizableInstance() {
        return previous;
    }

    protected void resetStaticPathAll(ResetIdSource source) {
        return;
    }

    public boolean resetPath(ResetIdSource source) {

        short resetId = source.getResetId();

        if(id == resetId)
            return false;

        id = resetId;
        currentField = (short)0;
        isDuplicated = false;

        return true;

    }

    protected void numberStaticPathAll(Output out) {
        return;
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

    protected short getClassId(Synchronizable instance) {

        if(instance instanceof AnotherTestClass1)
            return 0;

        if(instance instanceof SomeTestClass1)
            return 1;

        if(instance instanceof TestApplet1)
            return 2;

        return -1;

    }

    protected Synchronizable getNewInstance(Input in, short classId) {

        switch(classId) {
            case 0:
                return new AnotherTestClass1(in);

            case 1:
                return new SomeTestClass1(in);

            case 2:
                return new TestApplet1(in);
        }

        return null;

    }

    protected void serializeStaticAll(Output out)
        throws ISOException, UserException {
        TestApplet1.serializeStatic(out);
    }


    public void serialize(Output out)
        throws ISOException, UserException {

        switch(currentField) {
            case (short)0:
                out.write(someValue);
                out.resetResume();
                ++currentField;

            case (short)1:
                out.write(sArray, (short)0);
        }
        currentField = (short)0;

    }

    protected void mergeStaticAll(Input in)
        throws ISOException, UserException {
        TestApplet1.mergeStatic(in);
    }

    public void merge(Input in)
        throws ISOException, UserException {

        switch(currentField) {
            case (short)0:
                someValue = in.read(someValue);
                in.resetResume();
                ++currentField;

            case (short)1:
                sArray = in.checkArray(sArray);
                in.resetResume();
                ++currentField;

            case (short)2:
                in.read(sArray);
        }
        currentField = (short)0;

    }

}
