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
import synclib.SyncSW;
import synclib.Input;
import synclib.Output;
import synclib.ResetIdSource;
import synclib.Synchronizable;
import synclib.SynchronizableArrayFields;
import synclib.SynchronizableInstanceFields;

import javacard.framework.Applet;
import javacard.framework.ISOException;
import javacard.framework.APDU;
import javacard.framework.Shareable;
import javacard.framework.AID;
import javacard.framework.UserException;
import javacard.framework.JCSystem;
import javacard.framework.ISO7816;

public class TestApplet1 extends SuperTestApplet1 implements Synchronizable, SynchronizableArrayFields, SynchronizableInstanceFields {

    private static short stuff = (short)0x4141;
    private boolean bTrue = true;
    private boolean bFalse = false;
    private byte b = (byte)0x42;
    private short s = (short)0xDEAD;
    private final static short booleanArraySize = (short)16;
    private boolean[] booleanArray;
    private SomeTestClass1 someTestClass1 = null;

    private short currentField = (short)0;

    public TestApplet1() {

        booleanArray = new boolean[booleanArraySize];
        booleanArray[0] = true;
        booleanArray[1] = false;
        booleanArray[2] = true;
        booleanArray[3] = false;
        booleanArray[4] = true;
        booleanArray[5] = false;
        booleanArray[6] = true;
        booleanArray[7] = false;
        booleanArray[8] = true;
        booleanArray[9] = false;
        booleanArray[10] = true;
        booleanArray[11] = false;
        booleanArray[12] = true;
        booleanArray[13] = false;
        booleanArray[14] = true;
        booleanArray[15] = false;

        someTestClass1 = new SomeTestClass1();
        super.sArray = someTestClass1.sArray;

    }

    public TestApplet1(Input in) {
        if(in == null)
            ISOException.throwIt(ISO7816.SW_UNKNOWN);
    }

    public static void install(byte[] bArray, short bOffset, byte bLength)
        throws ISOException {

        if(bArray[bOffset] == (byte)0)
            new TestApplet1().syncRegister();
        else
            new TestApplet1().syncRegister(bArray, (short)(bOffset + 1), bArray[bOffset]);

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

            case (byte)0x01: /* super.anotherTestClass1.bArray */
                if(isSet) {
                    super.anotherTestClass1.bArray[(short)p1] = p2;
                } else if(getArraySize) {
                    buffer[0] = (byte)(super.anotherTestClass1.bArray.length >> 8);
                    buffer[1] = (byte)(super.anotherTestClass1.bArray.length & (short)0xFF);
                    apdu.setOutgoingAndSend((short)0, (short)2);
                } else if (setArraySize) {
                    byte[] newArray = null;
                    short newArraySize = (short)(p1 & 0xFF);
                    byte type = JCSystem.isTransient(super.anotherTestClass1.bArray);

                    if(type == JCSystem.NOT_A_TRANSIENT_OBJECT)
                        newArray = new byte[newArraySize];
                    else
                        newArray = JCSystem.makeTransientByteArray(newArraySize, type);

                    if(newArraySize > super.anotherTestClass1.bArray.length)
                        for(short i = 0; i < super.anotherTestClass1.bArray.length; i++)
                            newArray[i] = super.anotherTestClass1.bArray[i];
                    else
                        for(short i = 0; i < newArraySize; i++)
                            newArray[i] = super.anotherTestClass1.bArray[i];

                    super.anotherTestClass1.bArray = newArray;
                    JCSystem.requestObjectDeletion();
                } else if(isGet) {
                    buffer[0] = super.anotherTestClass1.bArray[(short)p1];
                    apdu.setOutgoingAndSend((short)0, (short)1);
                }
                break;

            case (byte)0x02: /* someTestClass1.sArray */
                if(isSet) {
                    someTestClass1.sArray[(short)p1] = (short)(((short)p2) & 0x00FF);
                } else if(getArraySize) {
                    buffer[0] = (byte)(someTestClass1.sArray.length >> 8);
                    buffer[1] = (byte)(someTestClass1.sArray.length & (short)0xFF);
                    apdu.setOutgoingAndSend((short)0, (short)2);
                } else if(setArraySize) {
                    short[] newArray = null;
                    short newArraySize = (short)(p1 & 0xFF);
                    byte type = JCSystem.isTransient(someTestClass1.sArray);

                    if(type == JCSystem.NOT_A_TRANSIENT_OBJECT)
                        newArray = new short[newArraySize];
                    else
                        newArray = JCSystem.makeTransientShortArray(newArraySize, type);

                    if(newArraySize > someTestClass1.sArray.length)
                        for(short i = 0; i < someTestClass1.sArray.length; i++)
                            newArray[i] = someTestClass1.sArray[i];
                    else
                        for(short i = 0; i < newArraySize; i++)
                            newArray[i] = someTestClass1.sArray[i];

                    someTestClass1.sArray = newArray;
                    JCSystem.requestObjectDeletion();
                } else if (isGet) {
                    buffer[0] = (byte)(someTestClass1.sArray[(short)p1] & 0xFF);
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

            case (byte)0x07: /* super.someValue */
                if(isSet) {
                    super.someValue = (short)((((short)p1) << 8)| (short)p2);
                } else if(isGet) {
                    buffer[0] = (byte)(super.someValue >> 8);
                    buffer[1] = (byte)(super.someValue & 0xFF);
                    apdu.setOutgoingAndSend((short)0, (short)2);
                }
        }

    }

    public void process(APDU apdu)
        throws ISOException {

        super.process(apdu);

        tweak_fields(apdu);

    }


    public boolean hasCurrentSynchronizableInstance() {
        return currentField < (short)2;
    }

    public void selectNextSynchronizableInstance() {

        if(currentField == (short)0) {
            super.selectNextSynchronizableInstance();
            if(!super.hasCurrentSynchronizableInstance())
                ++currentField;
            return;
        } else if(currentField < (short)2)
            ++currentField;

    }
    
    public Synchronizable getCurrentSynchronizableInstance()
        throws ClassCastException {

        switch(currentField) {
            case 0:
                return super.getCurrentSynchronizableInstance();
            case 1:
                return (Synchronizable)someTestClass1;
        }

        return null;

    }

    public void setCurrentSynchronizableInstance(Synchronizable instance) {

        switch(currentField) {
            case (short)0:
                super.setCurrentSynchronizableInstance(instance);
                return;

            case (short)1:
                someTestClass1 = (SomeTestClass1)instance;
                return;
        }

    }

    public void resetNextSynchronizableInstanceSelect() {
        super.resetNextSynchronizableInstanceSelect();
        currentField = (short)0;
    }

    public boolean resetPath(ResetIdSource source) {

        currentField = (short)0;

        return super.resetPath(source);

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

    public static void serializeStatic(Output out)
        throws ISOException, UserException {

        out.write(stuff);

    }

    public void serialize(Output out)
        throws ISOException, UserException {

        switch(currentField) {
            case (short)0:
                // staticaly determined (should be)
                super.serialize(out);
                out.resetResume();
                ++currentField;

            case (short)1:
                out.write(booleanArray, (short)0);
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
                out.resetResume();
                ++currentField;

            case (short)5:
                out.write(s);
        }
        currentField = (short)0;

    }


    public static void mergeStatic(Input in)
        throws ISOException, UserException {

        stuff = in.read(stuff);

    }


    public void merge(Input in)
        throws ISOException, UserException {

        switch(currentField) {
            case (short)0:
                //staticaly determined (should be)
                super.merge(in);
                in.resetResume();
                ++currentField;

            case (short)1:
                booleanArray = in.checkArray(booleanArray);
                in.resetResume();
                ++currentField;

            case (short)2:
                in.read(booleanArray);
                in.resetResume();
                ++currentField;

            case (short)3:
                bTrue = in.read(bTrue);
                in.resetResume();
                ++currentField;

            case (short)4:
                bFalse = in.read(bFalse);
                in.resetResume();
                ++currentField;

            case (short)5:
                b = in.read(b);
                in.resetResume();
                ++currentField;

            case (short)6:
                s = in.read(s);
        }
        currentField = (short)0;

    }

}
