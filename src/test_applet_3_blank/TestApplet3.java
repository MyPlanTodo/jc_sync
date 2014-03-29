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

package test_applet_3_blank;

import javacard.framework.Applet;
import javacard.framework.AppletEvent;
import javacard.framework.APDU;
import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.JCSystem;

public class TestApplet3 extends Applet implements AppletEvent {

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

    public static void install(byte[] bArray, short bOffset, byte bLength)
        throws ISOException {

        if(bArray[bOffset] == (byte)0)
            new TestApplet3().register();
        else
            new TestApplet3().register(bArray, (short)(bOffset + 1), bArray[bOffset]);

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

        tweak_fields(apdu);

    }

    public void uninstall() {

        someTestClass3 = null;
        if(anotherTestClass3 != null) {
            anotherTestClass3.someTestClass3 = null;
            anotherTestClass3 = null;
        }
        JCSystem.requestObjectDeletion();

    }
}
