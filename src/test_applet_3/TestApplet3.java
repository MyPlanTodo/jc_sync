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

package test_applet_3;

import synclib.SyncApplet;
import synclib.Input;
import synclib.Output;
import synclib.ResetIdSource;
import synclib.Synchronizable;
import synclib.SynchronizableArrayFields;
import synclib.SynchronizableInstanceFields;

import javacard.framework.Applet;
import javacard.framework.AppletEvent;
import javacard.framework.ISOException;
import javacard.framework.APDU;
import javacard.framework.Shareable;
import javacard.framework.AID;
import javacard.framework.UserException;
import javacard.framework.ISO7816;
import javacard.framework.JCSystem;

public class TestApplet3 extends SyncApplet implements Synchronizable, SynchronizableArrayFields, SynchronizableInstanceFields, AppletEvent {

    private boolean bTrue = true;
    private boolean bFalse = false;
    private byte b = (byte)0x42;
    private static short s = (short)0xDEAD;
    private final static short booleanArraySize = (short)32;
    private boolean[] booleanArray;
    private final static short sArraySize = (short)16;
    private short[] sArray;
    private SomeTestClass3 someTestClass3 = null;
    private static AnotherTestClass3 anotherTestClass3 = null;

    protected short id;
    protected boolean isDuplicated = false;

    private static short currentStaticField = (short)0;

    protected Synchronizable previous = null;
    private short currentField = (short)0;
    protected SynchronizableArrayFields next = null;

    public TestApplet3() {

        booleanArray = new boolean[booleanArraySize];
        for(short i = (short)0; i < booleanArraySize; i++)
            booleanArray[i] = ((i % 2) == 0);

        sArray = new short[sArraySize];
        for(short i = (short)0; i < sArraySize; i++)
            sArray[i] = i;

        someTestClass3 = new SomeTestClass3();

        if(anotherTestClass3 == null) {
            anotherTestClass3 = new AnotherTestClass3();
            anotherTestClass3.someTestClass3 = someTestClass3;
        }

    }

    public TestApplet3(Input in) {
        if(in == null)
            ISOException.throwIt(ISO7816.SW_UNKNOWN);
    }

    public static void install(byte[] bArray, short bOffset, byte bLength)
        throws ISOException {

        if(bArray[bOffset] == (byte)0)
            new TestApplet3().syncRegister();
        else
            new TestApplet3().syncRegister(bArray, (short)(bOffset + 1), bArray[bOffset]);

    }


    private void tweak_fields(APDU apdu) {

        byte[] buffer = apdu.getBuffer();
        byte ins = buffer[ISO7816.OFFSET_INS];
        boolean isSet = (ins >> 4) == (byte)0x00;
        boolean isGet = (ins >> 4) == (byte)0x01;
        boolean getArraySize = (ins >> 4) == (byte)0x02;
        boolean setArraySize = (ins >> 4) == (byte)0x03;
        byte p1 = buffer[ISO7816.OFFSET_P1];
        byte p2 = buffer[ISO7816.OFFSET_P2];

        if(ins == ISO7816.INS_SELECT)
            return;

        switch(ins & (byte)0x0F) {
            case (byte)0x00: /* booleanArray */
                if(isSet) {
                    booleanArray[(short)p1] = (p2 == (byte)1) ? true : false;
                } else if(getArraySize) {
                    buffer[0] = (byte)(booleanArray.length >> 8);
                    buffer[1] = (byte)(booleanArray.length & (short)0xFF);
                    apdu.setOutgoingAndSend((short)0, (short)2);
                } else if (setArraySize) {
                    boolean[] newArray = null;
                    short newArraySize = (short)(p1 & 0xFF);
                    byte type = JCSystem.isTransient(booleanArray);

                    if(type == JCSystem.NOT_A_TRANSIENT_OBJECT)
                        newArray = new boolean[newArraySize];
                    else
                        newArray = JCSystem.makeTransientBooleanArray(newArraySize, type);

                    if(newArraySize > booleanArray.length)
                        for(short i = 0; i < booleanArray.length; i++)
                            newArray[i] = booleanArray[i];
                    else
                        for(short i = 0; i < newArraySize; i++)
                            newArray[i] = booleanArray[i];

                    booleanArray = newArray;
                    JCSystem.requestObjectDeletion();
                } else if(isGet) {
                    buffer[0] = booleanArray[(short)p1] ? (byte)1 : (byte)0;
                    apdu.setOutgoingAndSend((short)0, (short)1);
                }
                break;

            case (byte)0x01: /* anotherTestClass3.bArray */
                if(isSet) {
                    anotherTestClass3.bArray[(short)p1] = p2;
                } else if(getArraySize) {
                    buffer[0] = (byte)(anotherTestClass3.bArray.length >> 8);
                    buffer[1] = (byte)(anotherTestClass3.bArray.length & (short)0xFF);
                    apdu.setOutgoingAndSend((short)0, (short)2);
                } else if (setArraySize) {
                    byte[] newArray = null;
                    short newArraySize = (short)(p1 & 0xFF);
                    byte type = JCSystem.isTransient(anotherTestClass3.bArray);

                    if(type == JCSystem.NOT_A_TRANSIENT_OBJECT)
                        newArray = new byte[newArraySize];
                    else
                        newArray = JCSystem.makeTransientByteArray(newArraySize, type);

                    if(newArraySize > anotherTestClass3.bArray.length)
                        for(short i = 0; i < anotherTestClass3.bArray.length; i++)
                            newArray[i] = anotherTestClass3.bArray[i];
                    else
                        for(short i = 0; i < newArraySize; i++)
                            newArray[i] = anotherTestClass3.bArray[i];

                    anotherTestClass3.bArray = newArray;
                    JCSystem.requestObjectDeletion();
                } else if(isGet) {
                    buffer[0] = anotherTestClass3.bArray[(short)p1];
                    apdu.setOutgoingAndSend((short)0, (short)1);
                }
                break;

            case (byte)0x02: /* sArray */
                if(isSet) {
                    sArray[(short)p1] = (short)(((short)p2) & 0x00FF);
                } else if(getArraySize) {
                    buffer[0] = (byte)(sArray.length >> 8);
                    buffer[1] = (byte)(sArray.length & (short)0xFF);
                    apdu.setOutgoingAndSend((short)0, (short)2);
                } else if(setArraySize) {
                    short[] newArray = null;
                    short newArraySize = (short)(p1 & 0xFF);
                    byte type = JCSystem.isTransient(sArray);

                    if(type == JCSystem.NOT_A_TRANSIENT_OBJECT)
                        newArray = new short[newArraySize];
                    else
                        newArray = JCSystem.makeTransientShortArray(newArraySize, type);

                    if(newArraySize > sArray.length)
                        for(short i = 0; i < sArray.length; i++)
                            newArray[i] = sArray[i];
                    else
                        for(short i = 0; i < newArraySize; i++)
                            newArray[i] = sArray[i];

                    sArray = newArray;
                    JCSystem.requestObjectDeletion();
                } else if (isGet) {
                    buffer[0] = (byte)(sArray[(short)p1] & 0xFF);
                    apdu.setOutgoingAndSend((short)0, (short)1);
                }
                break;

            case (byte)0x03: /*bTrue*/
                if(isSet) {
                    bTrue = (p2 == (byte)0) ? false : true;
                } else if(isGet) {
                    buffer[0] = bTrue ? (byte)1 : (byte)0;
                    apdu.setOutgoingAndSend((short)0, (short)1);
                }
                break;

            case (byte)0x04: /*bFalse*/
                if(isSet) {
                    bFalse = (p2 == (byte)0) ? false : true;
                } else if(isGet) {
                    buffer[0] = bFalse ? (byte)1 : (byte)0;
                    apdu.setOutgoingAndSend((short)0, (short)1);
                }
                break;

            case (byte)0x05: /* b */
                if(isSet) {
                    b = p2;
                } else if(isGet) {
                    buffer[0] = b;
                    apdu.setOutgoingAndSend((short)0, (short)1);
                }
                break;

            case (byte)0x06: /* s */
                if(isSet) {
                    s = (short)((((short)p1) << 8)| (short)p2);
                } else if(isGet) {
                    buffer[0] = (byte)(s >> 8);
                    buffer[1] = (byte)(s & 0xFF);
                    apdu.setOutgoingAndSend((short)0, (short)2);
                }
                break;

            case (byte)0x07: /* someTestClass3.s */
                if(isSet) {
                    someTestClass3.s = (short)((((short)p1) << 8)| (short)p2);
                    anotherTestClass3.someTestClass3.s = someTestClass3.s;
                } else if(isGet) {
                    buffer[0] = (byte)(someTestClass3.s >> 8);
                    buffer[1] = (byte)(anotherTestClass3.someTestClass3.s & 0xFF);
                    apdu.setOutgoingAndSend((short)0, (short)2);
                }
        }

    }

    public void process(APDU apdu)
        throws ISOException {

        super.process(apdu);

        tweak_fields(apdu);

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
                return (Synchronizable)someTestClass3;
        }
        return null;

    }


    public void setCurrentSynchronizableInstance(Synchronizable instance) {

        switch(currentField) {
            case (short)0:
                someTestClass3 = (SomeTestClass3)instance;
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
        TestApplet3.resetStaticPath(source);
    }

    public static void resetStaticPath(ResetIdSource source) {
        currentStaticField = (short)0;
        SyncApplet.resetPathAll(source, anotherTestClass3);
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


    protected void numberStaticPathAll(Output out) {
        TestApplet3.numberStaticPath(out);
    }

    public static void numberStaticPath(Output out) {
        SyncApplet.numberPathAll(out, anotherTestClass3);
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
        return 1;
    }

    public boolean[] getBooleanArray(short fieldId) {
        switch(fieldId) {
            case (short)0:
                return booleanArray;
        }
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

        if(instance instanceof SomeTestClass3)
            return 0;

        if(instance instanceof AnotherTestClass3)
            return 1;

        if(instance instanceof TestApplet3)
            return 2;

        return -1;

    }


    protected Synchronizable getNewInstance(Input in, short classId) {

        switch(classId) {
            case 0:
                return new SomeTestClass3(in);

            case 1:
                return new AnotherTestClass3(in);

            case 2:
                return new TestApplet3(in);
        }
        return null;

    }


    protected void serializeStaticAll(Output out)
        throws ISOException, UserException {

        TestApplet3.serializeStatic(out);

    }

    static void serializeStatic(Output out)
        throws ISOException, UserException {

        switch(currentStaticField) {
            case 0:
                if(anotherTestClass3 instanceof Synchronizable) {
                    SyncApplet.serializeAll(out, (Synchronizable)anotherTestClass3);
                } else {
                    out.skipInstance();
                }
                out.resetResume();
                ++currentStaticField;
            case 1:
                out.write(s);
        }

    }

    public void serialize(Output out)
        throws ISOException, UserException {

        switch(currentField) {
            case (short)0:
                out.write(booleanArray, (short)0);
                out.resetResume();
                ++currentField;

            case (short)1:
                out.write(sArray, (short)0);
                out.resetResume();
                ++currentField;

            case (short)2:
                out.write(bTrue);
                out.resetResume();
                ++currentField;

            case (short)3:
                out.write(bFalse);
                out.resetResume();
                ++currentField;

            case (short)4:
                out.write(b);
        }
        currentField = (short)0;

    }

    protected void mergeStaticAll(Input in)
        throws ISOException, UserException {

        TestApplet3.mergeStatic(in);

    }

    static void mergeStatic(Input in)
        throws ISOException, UserException {

        switch(currentStaticField) {
            case 0:
                try {
                    if(anotherTestClass3 instanceof Synchronizable)
                        anotherTestClass3 = (AnotherTestClass3)SyncApplet.mergeAll(in, (Synchronizable)anotherTestClass3);
                    else
                        anotherTestClass3 = (AnotherTestClass3)SyncApplet.mergeAll(in, null);
                }
                catch(UserException e) {
                    if(e.getReason() == Input.NEW_INSTANCE) {
                        anotherTestClass3 = (AnotherTestClass3)in.newlyCreatedInstance;
                        SyncApplet.mergeAll(in, anotherTestClass3);
                    } else if(e.getReason() != Input.SKIP_INSTANCE)
                        throw e;
                }
                in.resetResume();
                ++currentStaticField;

            case 1:
                s = in.read(s);
        }

    }

    public void merge(Input in)
        throws ISOException, UserException {

        switch(currentField) {
            case (short)0:
                booleanArray = in.checkArray(booleanArray);
                in.resetResume();
                ++currentField;

            case (short)1:
                in.read(booleanArray);
                in.resetResume();
                ++currentField;

            case (short)2:
                sArray = in.checkArray(sArray);
                in.resetResume();
                ++currentField;

            case (short)3:
                in.read(sArray);
                in.resetResume();
                ++currentField;

            case (short)4:
                bTrue = in.read(bTrue);
                in.resetResume();
                ++currentField;

            case (short)5:
                bFalse = in.read(bFalse);
                in.resetResume();
                ++currentField;

            case (short)6:
                b = in.read(b);
        }
        currentField = (short)0;

    }

    public void uninstall() {

        syncUninstall();
        someTestClass3 = null;
        if(anotherTestClass3 != null) {
            anotherTestClass3.someTestClass3 = null;
            anotherTestClass3 = null;
        }
        JCSystem.requestObjectDeletion();

    }
}
