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

package dock;

import com.sun.javacard.scriptgen.CAP;

import com.sun.javacard.apduio.Apdu;
import com.sun.javacard.apduio.CadClientInterface;
import com.sun.javacard.apduio.CadTransportException;

import java.io.Writer;
import java.io.StringWriter;
import java.io.PrintWriter;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.util.PropertyResourceBundle;
import java.io.File;
import java.io.InputStream;
import java.io.FileOutputStream;

/**
 * This class provides a simplified access to the scriptgen and apdutool tools for uploading CAP file on a cref javacard.
 */
public class CrefInstaller {

    /**
     * The uploaded CAP file.
     */
    private CAP CAPFile;

    /**
     * The buffer used to stock the generated script.
     */
    private StringWriter scriptWriter;

    /**
     * The log output.
     */
    private PrintWriter log = new PrintWriter(System.err, true);

    /**
     * The temp file used for fetching the CAP file from the InputStream.
     */
    private File file;

    /**
     * Was the CAP file processed and thus ready for upload ?
     */
    private boolean isProcessed = false;

    /**
     * Create an instance of the installer using the CAP file coming from the InputStream.
     * @param CAPFile An InputStream to read from for fetching the CAP file.
     * @throws IOException An IOException is thrown if something went wrong while reading the CAP File.
     * @throws FileNotFoundException A FileNotFoundException is thrown if the temp file cannot be found.
     */
    public CrefInstaller(InputStream CAPFile)
        throws IOException, FileNotFoundException {

        file = File.createTempFile("temp", ".cap");
        FileOutputStream outputFile = new FileOutputStream(file);
        byte[] buffer = new byte[1024];
        int nbRead = -1;
        while((nbRead = CAPFile.read(buffer)) != -1)
            outputFile.write(buffer, 0, nbRead);
        CAPFile.close();

        try {
            this.CAPFile = new CAP(file.getAbsolutePath(), log, PropertyResourceBundle.getBundle("com/sun/javacard/scriptgen/MessagesBundle"), null);
        }
        catch(Exception e) {
            System.err.println("Installer: " + e.getMessage());
            return;
        }
        scriptWriter = new StringWriter();

    }


    /**
     * Process the CAP file using scriptgen.
     * @return Return true if successful, false else.
     */
    public boolean processCAPFile() {

        if(isProcessed)
            return true;

        if(CAPFile.verifyCAP() != 0)
            return false;
        PrintWriter printWriter = new PrintWriter((Writer)scriptWriter);
        CAPFile.genScript(printWriter, false);
        printWriter.flush();
        log.flush();
        file.delete();
        isProcessed = true;

        return true;

    }


    /**
     * Take two byte to make a short.
     * @param b1 The byte of high order.
     * @param b2 The byte of low order.
     * @return The built short.
     */
    private short makeShort(byte b1, byte b2) {

        return (short)(((b1 & 0xFF) << 8) | (b2 & 0xFF));

    }


    /**
     * Send an apdu to the device to abort the current upload.
     * @param device The device to send the apdu to.
     */
    private void abortUpload(CadClientInterface device) {

        Apdu apdu = new Apdu();
        apdu.command[Apdu.CLA] = (byte)0x80;
        apdu.command[Apdu.INS] = (byte)0xb8;
        apdu.command[Apdu.P1] = (byte)0x00;
        apdu.command[Apdu.P2] = (byte)0x00;
        apdu.setLc(0);
        apdu.setLe(0);

        try {
            device.exchangeApdu(apdu);
        }
        catch(IOException e) {
            System.err.println("Cannot send an abort APDU");
        }
        catch(CadTransportException e) {
            System.err.println(e.getMessage());
        }

    }


    /**
     * Upload the processed CAP file as a sequence of apdu like apdutool would do.
     * @param device The device to upload the CAP file to.
     * @return Return true if successful, false else.
     */
    public boolean uploadCAP(CadClientInterface device) {

        String script = scriptWriter.toString();
        int i = 0;

        String lineSeparator = System.getProperty("line.separator", "\n");

        while(i < script.length()) {

            if(script.charAt(i) == '/') {
                i = script.indexOf(System.getProperty("line.separator", "\n"), i);
                if(i == -1) {
                    abortUpload(device);
                    return false;
                }
                continue;
            }

            if(script.indexOf(System.getProperty("line.separator", "\n"), i) == i) {
                i += lineSeparator.length();
                continue;
            }

            byte CLA = Integer.decode(script.substring(i, i + 4)).byteValue();
            i += 5;
            byte INS = Integer.decode(script.substring(i, i + 4)).byteValue();
            i += 5;
            byte P1 = Integer.decode(script.substring(i, i + 4)).byteValue();
            i += 5;
            byte P2 = Integer.decode(script.substring(i, i + 4)).byteValue();
            i += 5;
            byte Lc = Integer.decode(script.substring(i, i + 4)).byteValue();
            i += 5;
            byte[] data = null;

            if(Lc != (byte)0) {
                data = new byte[(short)Lc & (short)0x00FF];
                for(int j = 0; j < data.length; j++) {
                    data[j] = Integer.decode(script.substring(i, i + 4)).byteValue();
                    i += 5;
                }
            }

            i += 4;
            if(script.charAt(i) != ';') {
                abortUpload(device);
                return false;
            }
            i++;

            Apdu apdu = new Apdu();
            apdu.command[Apdu.CLA] = CLA;
            apdu.command[Apdu.INS] = INS;
            apdu.command[Apdu.P1] = P1;
            apdu.command[Apdu.P2] = P2;
            if(data != null)
                apdu.setDataIn(data, data.length);
            else
                apdu.setLc(0);
            apdu.setLe(0);


            try {
                device.exchangeApdu(apdu);
            }
            catch(IOException e) {
                System.err.println("Cannot send a download APDU");
                abortUpload(device);
                return false;
            }
            catch(CadTransportException e) {
                System.err.println(e.getMessage());
                abortUpload(device);
                return false;
            }

            byte[] sw1Sw2 = apdu.getSw1Sw2();
            short sw = makeShort(sw1Sw2[0], sw1Sw2[1]);
            if(sw != (short)0x9000) {
                System.err.println("Response status: " + (sw1Sw2[0] < 16 ? "0x0" : "0x") + Integer.toHexString(sw1Sw2[0]) + (sw1Sw2[1] < 16 ? "0" : "") + Integer.toHexString(sw1Sw2[1]));
                abortUpload(device);
                return false;
            }

        }

        System.out.println("Uploading done");

        return true;

    }

}
