import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import AnimatedNumber from 'animated-number-react';
import './InfoBox.scss';
import Card from '../Card';

export default (props) => {
    return (
        <Card className={'info-box'}>
            <FontAwesomeIcon className='fa' icon={props.icon} />
            <div className='content'>
                <p className='text-muted mt-2 mb-0'>{props.title}</p>
                <p className='text-primary text-24 line-height-1 mb-2'>
                    {!isNaN (props.value) && (<><AnimatedNumber value={props.value} formatValue={(value) => value.toFixed(1)} duration={1000} /><small>{props.unit}</small></>)}
                    {isNaN(props.value) && <>{props.value}</>}
                </p>
            </div>
        </Card>
    );
};