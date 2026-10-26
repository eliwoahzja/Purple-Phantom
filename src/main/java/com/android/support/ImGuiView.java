package com.android.support;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.text.InputType;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.WindowInsets;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/** All native calls are serialized on the GL thread, including input. */
public final class ImGuiView extends GLSurfaceView implements GLSurfaceView.Renderer {
    private int pointerId = -1;
    private int surfaceWidth = 1, surfaceHeight = 1;
    private boolean keyboardShown = false;

    private static native void nativeCreate();
    private static native boolean nativeFrame(int width, int height);
    private static native void nativeDestroy();
    private static native void nativeTouch(int action, float x, float y);
    private static native void nativeText(String text);
    private static native void nativeKey(int key, boolean down);
    private static native void nativeScroll(float x, float y);
    private static native void nativeBottomInset(int pixels);

    public ImGuiView(Context context) {
        super(context);
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 0, 0);
        setPreserveEGLContextOnPause(false);
        setFocusable(true);
        setFocusableInTouchMode(true);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        requestFocus();
    }

    @Override public void onSurfaceCreated(GL10 gl, EGLConfig config) { nativeCreate(); }
    @Override public WindowInsets onApplyWindowInsets(WindowInsets insets) {
        // Keep the app's swipe target above Android's mandatory home gesture area.
        final int bottom = Build.VERSION.SDK_INT >= 29
                ? insets.getMandatorySystemGestureInsets().bottom : 0;
        queueEvent(new Runnable() {
            @Override public void run() { nativeBottomInset(bottom); }
        });
        return super.onApplyWindowInsets(insets);
    }
    @Override public void onSurfaceChanged(GL10 gl, int width, int height) {
        surfaceWidth = Math.max(1, width);
        surfaceHeight = Math.max(1, height);
    }
    @Override public void onDrawFrame(GL10 gl) {
        final boolean wantsKeyboard = nativeFrame(surfaceWidth, surfaceHeight);
        if (wantsKeyboard != keyboardShown) {
            keyboardShown = wantsKeyboard;
            post(new Runnable() {
                @Override public void run() {
                    InputMethodManager input = (InputMethodManager)getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                    if (input == null) return;
                    if (wantsKeyboard) input.showSoftInput(ImGuiView.this, InputMethodManager.SHOW_IMPLICIT);
                    else input.hideSoftInputFromWindow(getWindowToken(), 0);
                }
            });
        }
    }
    public void releaseForPause() {
        pointerId = -1;
        queueEvent(new Runnable() {
            @Override public void run() { nativeDestroy(); keyboardShown = false; }
        });
    }

    @Override public boolean onTouchEvent(MotionEvent event) {
        int action = event.getActionMasked();
        if (action == MotionEvent.ACTION_DOWN) {
            requestFocus();
            pointerId = event.getPointerId(0);
        }
        int index = event.findPointerIndex(pointerId);
        if (action == MotionEvent.ACTION_POINTER_UP &&
                event.getPointerId(event.getActionIndex()) == pointerId) {
            index = event.getActionIndex();
            action = MotionEvent.ACTION_UP;
        }
        if (action == MotionEvent.ACTION_CANCEL) {
            pointerId = -1;
            queueEvent(new Runnable() {
                @Override public void run() { nativeTouch(MotionEvent.ACTION_CANCEL, 0, 0); }
            });
            return true;
        }
        if (index < 0) return true;
        if (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_MOVE || action == MotionEvent.ACTION_UP) {
            // View coordinates and framebuffer coordinates can differ on scaled surfaces.
            final float x = event.getX(index) / Math.max(1, getWidth());
            final float y = event.getY(index) / Math.max(1, getHeight());
            final int kind = action;
            queueEvent(new Runnable() {
                @Override public void run() { nativeTouch(kind, x * surfaceWidth, y * surfaceHeight); }
            });
        }
        if (action == MotionEvent.ACTION_UP) { pointerId = -1; performClick(); }
        return true;
    }
    @Override public boolean performClick() { super.performClick(); return true; }
    @Override public boolean onGenericMotionEvent(MotionEvent event) {
        if (event.getActionMasked() == MotionEvent.ACTION_SCROLL) {
            final float x = event.getAxisValue(MotionEvent.AXIS_HSCROLL);
            final float y = event.getAxisValue(MotionEvent.AXIS_VSCROLL);
            queueEvent(new Runnable() {
                @Override public void run() { nativeScroll(x, y); }
            });
            return true;
        }
        return super.onGenericMotionEvent(event);
    }
    @Override public boolean onCheckIsTextEditor() { return true; }
    @Override public InputConnection onCreateInputConnection(EditorInfo info) {
        info.inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS;
        info.imeOptions = EditorInfo.IME_FLAG_NO_EXTRACT_UI | EditorInfo.IME_ACTION_DONE;
        return new BaseInputConnection(this, false) {
            @Override public boolean commitText(CharSequence value, int cursor) {
                final String text = value.toString();
                queueEvent(new Runnable() {
                    @Override public void run() { nativeText(text); }
                });
                return true;
            }
            @Override public boolean deleteSurroundingText(final int before, final int after) {
                queueEvent(new Runnable() {
                    @Override public void run() {
                        for (int i=0; i<before; ++i) { nativeKey(KeyEvent.KEYCODE_DEL, true); nativeKey(KeyEvent.KEYCODE_DEL, false); }
                        for (int i=0; i<after; ++i) { nativeKey(KeyEvent.KEYCODE_FORWARD_DEL, true); nativeKey(KeyEvent.KEYCODE_FORWARD_DEL, false); }
                    }
                });
                return true;
            }
            @Override public boolean performEditorAction(int action) {
                queueEvent(new Runnable() {
                    @Override public void run() { nativeKey(KeyEvent.KEYCODE_ENTER, true); nativeKey(KeyEvent.KEYCODE_ENTER, false); }
                });
                return true;
            }
            @Override public boolean sendKeyEvent(KeyEvent event) { return ImGuiView.this.dispatchKeyEvent(event); }
        };
    }
    @Override public boolean dispatchKeyEvent(KeyEvent event) {
        if (event.getKeyCode() == KeyEvent.KEYCODE_BACK) return super.dispatchKeyEvent(event);
        final int key = event.getKeyCode();
        final boolean down = event.getAction() == KeyEvent.ACTION_DOWN;
        queueEvent(new Runnable() {
            @Override public void run() { nativeKey(key, down); }
        });
        if (down && event.getUnicodeChar() != 0 && !event.isCtrlPressed()) {
            final String text = new String(Character.toChars(event.getUnicodeChar()));
            queueEvent(new Runnable() {
                @Override public void run() { nativeText(text); }
            });
        }
        return true;
    }
}
