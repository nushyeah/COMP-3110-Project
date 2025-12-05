/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 */
import type {
  Element,
  ActivitySliceFilter,
  ComponentFilter,
} from 'react-devtools-shared/src/frontend/types';
import typeof {
  SyntheticMouseEvent,
  SyntheticKeyboardEvent,
} from 'react-dom-bindings/src/events/SyntheticEvent';
import type Store from 'react-devtools-shared/src/devtools/store';

import * as React from 'react';
import {useContext, useMemo, useTransition} from 'react';
import {
  ComponentFilterActivitySlice,
  ElementTypeActivity,
} from 'react-devtools-shared/src/frontend/types';
import styles from './ActivityList.css';
import {
  TreeStateContext,
  TreeDispatcherContext,
} from '../Components/TreeContext';
import {useHighlightHostInstance} from '../hooks';
import {StoreContext} from '../context';

export function useChangeActivitySliceAction(): (
  id: Element['id'] | null,
) => void {
  const store = useContext(StoreContext);

  function changeActivitySliceAction(activityID: Element['id'] | null) {
    const nextFilters: ComponentFilter[] = [];
    // Remove any existing activity slice filter
    for (let i = 0; i < store.componentFilters.length; i++) {
      const filter = store.componentFilters[i];
      if (filter.type !== ComponentFilterActivitySlice) {
        nextFilters.push(filter);
      }
    }

    if (activityID !== null) {
      const rendererID = store.getRendererIDForElement(activityID);
      if (rendererID === null) {
        throw new Error('Expected to find renderer.');
      }
      const activityFilter: ActivitySliceFilter = {
        type: ComponentFilterActivitySlice,
        activityID,
        rendererID,
        isValid: true,
        isEnabled: true,
      };
      nextFilters.push(activityFilter);
    }
    store.componentFilters = nextFilters;
  }

  return changeActivitySliceAction;
}

