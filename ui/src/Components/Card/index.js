import React from 'react';
import './Card.scss';

export default (props) => {
    return (
        <div className={'card ' + (props.className || '')} style={props.style || {}} onClick={() => {
                if (props.onClick)
                    props.onClick();
            }}>
            <div className='card-body'>
                {props.children}
            </div>
        </div>
    );
};