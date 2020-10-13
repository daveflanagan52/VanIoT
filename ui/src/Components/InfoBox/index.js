import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import './InfoBox.scss';
import Card from '../Card';

export default (props) => {
    return (
        <Card className={'info-box'}>
            <FontAwesomeIcon className='fa' icon={props.icon} />
            <div className='content'>
                <p className='text-muted mt-2 mb-0'>{props.title}</p>
                <p className='text-primary text-24 line-height-1 mb-2'>{props.value}{!isNaN(props.value) && <small>{props.unit}</small>}</p>
            </div>
        </Card>
    );
};