//
//  Created by Bradley Austin Davis 2016/01/20
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "Preferences.h"


void Preferences::addPreference(Preference* preference) {
    preference->setParent(this);

    const QString& category = preference->getCategory();

    // Use this structure to maintain the order of the categories
    if (!_categories.contains(category)) {
        _categories.append(category);
    }
    
    QVariantList categoryPreferences;
    // FIXME is there an easier way to do this with less copying?
    if (_preferencesByCategory.contains(category)) {
        categoryPreferences = qvariant_cast<QVariantList>(_preferencesByCategory[category]);
    } 
    categoryPreferences.append(QVariant::fromValue(preference));
    _preferencesByCategory[category] = categoryPreferences;
}

void Preference::setEnabler(BoolPreference* enabler, bool inverse) {
    if (_enabler) {
        disconnect(_enabler);
        _enabler = nullptr;
    }

    _enabler = enabler;
    _enablerInverted = inverse;
    if (_enabler) {
        connect(_enabler, &BoolPreference::valueChanged, this, &Preference::onEnablerValueChanged);
        onEnablerValueChanged(_enabler->getValue());
    }
}

void Preference::onEnablerValueChanged(int newValue) {
    bool value = (bool) newValue;
    if (_enablerInverted) {
        value = !value;
    }
    setEnabled(value);
}

void Preference::setEnablerInt(IntPreference* enabler, int index, bool inverse) {
    if (_enablerInt) {
        disconnect(_enablerInt);
        _enablerInt = nullptr;
    }
    
    _enablerInt = enabler;
    _enablerValue = index;
    _enablerInverted = inverse;
    if (_enablerInt) {
        connect(_enablerInt, &IntPreference::valueChanged, this, &Preference::onEnablerIntValueChanged);
        onEnablerIntValueChanged(_enablerInt->getValue());
    }
}

void Preference::onEnablerIntValueChanged(int value) {
    bool activation = value == _enablerValue;
    if (_enablerInverted) {
        activation = !activation ;
    }
    setEnabled(activation);
}
