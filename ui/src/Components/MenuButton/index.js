import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';

import './MenuButton.scss';

export default (props) => {
    return (
        <div className={'menu-button ' + (props.selected ? 'selected' : '')} onClick={() => props.onClick()}>
            <span className='icon'><FontAwesomeIcon icon={props.data.icon} /></span>
        </div>
    );
};