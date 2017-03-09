/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.fragment;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.GridView;
import android.widget.ListView;
import android.widget.TextView;

import org.easydarwin.easyclient.R;

/**
 * Created by Kim on 2016/6/18.
 */
public class BaseFragment extends Fragment {
    private boolean loaded = false;
    private boolean mTipContentExpanded;

    @Override
    public void setUserVisibleHint(boolean isVisibleToUser) {
        super.setUserVisibleHint(isVisibleToUser);
        if (isVisibleToUser && isVisible() && !loaded) {
            lazyLoad();
            loaded = true;
        }
    }

    @Override
    public void onStart() {
        super.onStart();
        if (getUserVisibleHint() && !loaded) {
            lazyLoad();
            loaded = true;
        }
    }

    /**
     * 延迟加载
     * 子类必须重写此方法
     */
    protected void lazyLoad() {
    }

    @Override
    public void onViewCreated(final View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        view.findViewById(R.id.tip_title).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final View content = view.findViewById(R.id.tip_content_container);
                content.setVisibility(View.VISIBLE);
                content.animate().cancel();
                content.animate().setListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationCancel(Animator animation) {
                        super.onAnimationCancel(animation);
                        if (mTipContentExpanded) {
                            content.setTranslationY(0);
                        } else {
                            content.setTranslationY(-content.getHeight());
                        }
                    }
                });
                if (!mTipContentExpanded) {
                    content.animate().translationY(0);
                } else {
                    content.animate().translationY(-content.getHeight());
                }
                mTipContentExpanded = !mTipContentExpanded;

            }
        });

        final View content = view.findViewById(R.id.tip_content_container);
        content.setVisibility(View.INVISIBLE);
        mTipContentExpanded = false;
        content.post(new Runnable() {
            @Override
            public void run() {
                content.setTranslationY(-content.getHeight());
            }
        });
        onSetTipContent((TextView) content.findViewById(R.id.tip_content));

        view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mTipContentExpanded) {
                    content.animate().translationY(-content.getHeight());
                }
            }
        });
    }

    protected void onSetTipContent(TextView tipContent) {

    }
}