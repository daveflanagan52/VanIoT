import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import InputRange from 'react-input-range';
import { faToggleOff, faToggleOn, faLightbulb, faFan, faHeat, faOutlet, faCarBattery, faMapMarked, faLocation } from '@fortawesome/pro-light-svg-icons';

import './DeviceDetails.scss';

const icons = {
    'lightbulb': faLightbulb,
    'fan': faFan,
    'heat': faHeat,
    'outlet': faOutlet,
    'car-battery': faCarBattery,
    'map-marked': faMapMarked,
    'location': faLocation,
};

export default (props) => {
    const capabilities = Object.entries(props.capabilities).map(arr => {
        switch(arr[1]) {
            case 'percent':
                return (
                    <span key={arr[0]} className='percent'>
                        <InputRange
                            value={props.state[arr[0]]}
                            minValue={0}
                            maxValue={255}
                            onChange={value => props.setDeviceState(props.id, arr[0], value)}
                            />
                    </span>
                );
            case 'bool':
                return (
                    <FontAwesomeIcon 
                        key={arr[0]} 
                        className={props.state[arr[0]] ? 'toggle on' : 'toggle'} 
                        icon={props.state[arr[0]] ? faToggleOn : faToggleOff}
                        onClick={() => props.setDeviceState(props.id, arr[0], props.state[arr[0]] ? 0 : 1)}
                        />
                );
            default:
                return null;
        }
    })

    return (
        <div className='row device-details align-items-center'>
            <div className='col col-icon'><FontAwesomeIcon className='icon fa' icon={icons[props.icon]} /></div>
            <div className='col-2'>{props.name}</div>
            <div className='col'>
                <div className='d-flex align-items-center capabilities'>
                    {capabilities}
                </div>
            </div>
        </div>
    );
};