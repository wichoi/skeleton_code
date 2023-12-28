package aprotech.ev.unimanager.fragment;

import android.app.Dialog;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.DialogFragment;

import aprotech.ev.unimanager.R;

public class DisconnectDialogFragment extends DialogFragment {
    public static final String TAG = "UniDisconn";
    private static final String ARG_TEXT = "ARG_TEXT";

    public static DisconnectDialogFragment getInstance(final int disconResId) {
        final DisconnectDialogFragment fragment = new DisconnectDialogFragment();

        final Bundle args = new Bundle();
        args.putInt(ARG_TEXT, disconResId);
        fragment.setArguments(args);

        return fragment;
    }

    @Override
    @NonNull
    public Dialog onCreateDialog(final Bundle savedInstanceState) {
        Log.v(TAG, "onCreateDialog()");
        final Bundle args = requireArguments();
        final StringBuilder text = new StringBuilder(getString(args.getInt(ARG_TEXT)));

        return new AlertDialog.Builder(requireContext())
                .setTitle(R.string.disconnect_title)
                .setMessage(text)
                .setPositiveButton(R.string.ok, null)
                .create();
    }

    public void show() {
        this.show();
    }
}
