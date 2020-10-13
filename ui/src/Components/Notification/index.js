import React from 'react';
import Moment from 'react-moment';

import './Notification.scss';

export default (props) => {
    return (
        <div className={'mb-3 alert alert-' + props.type}>
            <strong className='mr-2'>
                <Moment format='D.M.YY HH:mm'>
                    {props.created}
                </Moment>
            </strong>
            {props.message}
        </div>
    );
};