import React from 'react';
import './ContentView.scss';

export default (props) => {
    return (
        <div className='content-view'>
            {props.children}
        </div>
    );
};